#include <stdint.h>
#include <math.h>
#include <Wire.h>
#include "MAX30100.h"
#include "TMP006.h"

//Bluetooth communtcation
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(2, 3);

#define LM_SIZE      25  //Number of samples used to calculate the running mean difference
#define BT_BAUD      9600
#define SERIAL_BAUD  57600

//Where will the data be sent to
enum states {usb,
             bt,
             serialPlotter,
             other
            };
states Mode;

//Variables store raw RED and IR values
uint16_t rawIRValue = 0;
uint16_t rawRedValue = 0;

//Mean difference variables
uint16_t LM[LM_SIZE];      // LastMeasurements
uint8_t index = 0;
long sum = 0;
uint8_t count = 0;
long avg = 0;

//TMP006 rawSensorVoltage moving average variables
float meanVoltage[10];
float sumVoltage;

//uint16_t loopCounter_MAX30100 = 0;
//uint16_t loopCounter_TMP006 = 0;

uint16_t smooth_IR;
uint16_t smooth_RED;

//AC extraction variables (As done is the getDC function of the MAX30100_Main program
float red_w = 0;
float ir_w = 0;
float irAC;
float irDC;
float redAC;
float redDC;

//Timming variables
long prev_Micros_MAX30100 = 0;
long prev_Micros_TMP006 = 0;
long prev_Micros_ADS1299 = 0;
int counter = 0;

//Red LED current bias check variables
uint32_t lastBiasCheck = 0;
int redLedCurrent = i50;
int irLedCurrent = i50;

void setup()
{
  Wire.begin();
  bluetooth.begin(BT_BAUD);
  delay(2000); //Delay for bluetooth to begin (check if this is neccessary)
  TMP006_begin(TMP006_CFG_16SAMPLE);       //Samples taken per temp calculation
  MAX30100_begin(pw1600, i50, i50, sr50); //Mode(HR or SPO2), Pulse width, IR current, RED current, Sample rate
  MAX30100_clearFIFO();
  Serial.begin(SERIAL_BAUD);
  while (!Serial);

  Mode = usb;
  //serialPlotter

  //bluetooth.write("Connected to bluetooth device");
  //bluetooth.write("");
  Serial.println("\n\nTesting Testing Testing...");
  Serial.println("Programm started");
}

void loop()
{
  //------------------------------ADS1299 (execute once every 4 milliseconds)------------------------------
  if (micros() - prev_Micros_ADS1299 >= 4000)
  {
    //Read ADS1299 every 4ms
    //Serial.println("e_0");// Serial.println("0");
    //bluetooth.println("e_0");// Serial.println("0");
    prev_Micros_ADS1299 = micros();
  }


  //------------------------------MAX30100 (execute once every 20 milliseconds)------------------------------
  if (micros() - prev_Micros_MAX30100 >= 21000)
  {
    counter++;
    //Serial.print("***");
    //Serial.println(micros() - prev_Micros_MAX30100);
    redLedCurrent = MAX30100_CurrentBalancing(redLedCurrent); //Check the difference between raw IR and RED values then adjusts the current to remove some of the DC offset
    MAX30100_readSensor();

    prev_Micros_MAX30100 = micros();

    MAX30100_DCRemover();                     //Extract AC and DC values from raw IR and RED siganls
    //MAX30100_SpO2Calculator(irAC, redAC);     //SpO2 Calculation

    switch (Mode)
    {
      case usb:
        Serial.print("i_"); Serial.println(rawIRValue);
        Serial.print("r_"); Serial.println(rawRedValue);
        break;
      case bt:
        bluetooth.print("i_"); bluetooth.println(rawIRValue);
        bluetooth.print("r_"); bluetooth.println(rawRedValue);
        break;
      case serialPlotter:
        Serial.print(rawIRValue); Serial.print(","); Serial.println(rawRedValue);
        break;
      case other:
        break;
    }

  }

  //------------------------------TMP006 (execute once every 4 seconds)------------------------------
  if (micros() - prev_Micros_TMP006 >= 4000000)
  {
    float object_temp = TMP006_calcObjTempC();
    float ambient_temp = TMP006_readDieTempC();
    float sensor_voltage = TMP006_readRawVoltage();
    prev_Micros_TMP006 = micros();

    switch (Mode)
    {
      case usb:
        Serial.print("d_"); Serial.println(ambient_temp);
        Serial.print("v_"); Serial.println(sensor_voltage);
        Serial.print("o_"); Serial.println(object_temp);
        break;
      case bt:
        bluetooth.print("a_"); bluetooth.println(ambient_temp);
        bluetooth.print("o_"); bluetooth.println(object_temp);
        break;
      case serialPlotter:
        //Serial.print(","); Serial.print(ambient_temp); Serial.print(","); Serial.print(object_temp);
        break;
      case other:
        break;
    }
  }

  if (Mode == serialPlotter)
  {
    //Serial.println();
  }
}


