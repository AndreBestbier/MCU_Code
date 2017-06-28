
// Sensor address
#define MAX30100_ADDRESS        0x57  // 8bit address converted to 7bit

// Register addresses
#define MAX30100_INT_STATUS     0x00  // Which interrupts are tripped
#define MAX30100_INT_ENABLE     0x01  // Which interrupts are active
#define MAX30100_FIFO_WR_PTR    0x02  // Where data is being written
#define MAX30100_OVRFLOW_CTR    0x03  // Number of lost samples
#define MAX30100_FIFO_RD_PTR    0x04  // Where to read from
#define MAX30100_FIFO_DATA      0x05  // Output data buffer
#define MAX30100_MODE_CONFIG    0x06  // Control register
#define MAX30100_SPO2_CONFIG    0x07  // Oximetry settings
#define MAX30100_LED_CONFIG     0x09  // Pulse width and power of LEDs
#define MAX30100_TEMP_INTG      0x16  // Temperature value, whole number
#define MAX30100_TEMP_FRAC      0x17  // Temperature value, fraction
#define MAX30100_REV_ID         0xFE  // Part revision
#define MAX30100_PART_ID        0xFF  // Part ID, normally 0x11

//Mode
#define HR     0x02    //HR only enabled
#define SPO2   0x03    //SPO2 enabled

// LED Pulse Width Control
#define pw200  0    // 200us pulse
#define pw400  1    // 400us pulse
#define pw800  2    // 800us pulse
#define pw1600 3    // 1600us pulse

// SpO2 Sample Rate Control
#define sr50   0   // 50 samples per second
#define sr100  1   // 100 samples per second
#define sr167  2   // 167 samples per second
#define sr200  3   // 200 samples per second
#define sr400  4   // 400 samples per second
#define sr600  5   // 600 samples per second
#define sr800  6   // 800 samples per second
#define sr1000 7   // 1000 samples per second

// LED Current Control 
#define i0  0   // No current
#define i4  1   // 4.4mA
#define i8  2   // 7.6mA
#define i11 3   // 11.0mA
#define i14 4   // 14.2mA
#define i17 5   // 17.4mA
#define i21 6   // 20.8mA
#define i24 7   // 24.0mA
#define i27 8   // 27.1mA
#define i31 9   // 30.6mA
#define i34 10  // 33.8mA
#define i37 11  // 37.0mA
#define i40 12  // 40.2mA
#define i44 13  // 43.6mA
#define i47 14  // 46.8mA
#define i50 15  // 50.0mA

//Other Constants
#define CURRENT_ADJUSTMENT_PERIOD_MS        1000 //Adjust the current each 500 ms
#define SAMPLES_IN_SPO2_CALCULATION         150  //Numabe of AC samples used to calculate the SPO2 level


//SpO2 calculation variables
long redACsqrSum;
long irACsqrSum;
int acSqrSum_Counter;
float r_SpO2;

// MAX30100 Functions
void     MAX30100_setLEDs(uint8_t pw, uint8_t red, uint8_t ir);                           // Sets the LED state
void     MAX30100_begin(uint8_t pw, uint8_t ir, uint8_t red, uint8_t sr);   // Set start conditions in terms of Pulse width, IR and RED currents and Sampe rate (initialises in SpO2 mode)

void     MAX30100_setSPO2(uint8_t sr);                                                    // Setup the SPO2 sensor, disabled by default
int      MAX30100_getNumSamp(void);                                                       // Get number of samples
void     MAX30100_readSensor(void);                                                       // Updates the values
void     MAX30100_shutdown(void);                                                         // Instructs device to power-save
void     MAX30100_reset(void);                                                            // Resets the device
void     MAX30100_startup(void);                                                          // Leaves power-save
int      MAX30100_getRevID(void);                                                         // Gets revision ID
int      MAX30100_getPartID(void);                                                        // Gets part ID
void     MAX30100_printRegisters(void);                                                   // Dumps contents of registers for debug

//Filter and calculation functions
int      MAX30100_meanDiff(int M, int n);                                                 // Calculate the mean difference of the previous n samples
void     MAX30100_DCRemover ();
uint8_t  MAX30100_CurrentBalancing(int redLedCurrent);
void     MAX30100_SpO2Calculator(int irAC, int redAC);
void     MAX30100_beatDetection();

// I2C Functions
void    MAX30100_I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data);
uint8_t MAX30100_I2CreadByte(uint8_t address, uint8_t subAddress);
void    MAX30100_I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);
