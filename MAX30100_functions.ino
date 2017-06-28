
//Set start conditions in terms of Pulse width, IR and RED currents and Sampe rate 
void MAX30100_begin(uint8_t pw, uint8_t irLedCurrent, uint8_t redLedCurrent, uint8_t sr)
{                                                                       //(initialises in SpO2 mode)
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, 0x03);           // 0x02 -> HR only, 0x03 -> SpO2
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_LED_CONFIG, (redLedCurrent << 4) | irLedCurrent); // 4 bits for RED current and 4 bits for IR current
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, (1 << 6 | sr << 2 | pw)); //;(0x01 << 2 | 0x03)
  
  //uint8_t previous = MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG);
  //MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, previous | (1 << 6));
}

int MAX30100_getNumSamp(void)    //Number of samples in the FIFO
{
  uint8_t wrPtr = MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_FIFO_WR_PTR); //Points to where MAX30100 writes next sample
  uint8_t rdPtr = MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_FIFO_RD_PTR); //Points to location from where processor gets next sample from FIFO via I2C
  return (abs( 16 + wrPtr - rdPtr ) % 16);
}

void MAX30100_readSensor(void)   //Pop a sensor value from the FIFO
{
  uint8_t temp[4] = {0};  // Temporary buffer for read values
  MAX30100_I2CreadBytes(MAX30100_ADDRESS, MAX30100_FIFO_DATA, &temp[0], 4);  // Read four times from the FIFO
  rawIRValue = (temp[0] << 8) | temp[1];  // Combine values to get the actual number
  rawRedValue = (temp[2] << 8) | temp[3]; // Combine values to get the actual number  
}

void MAX30100_shutdown(void)
{
  uint8_t reg = MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG);  // Get the current register
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, reg | 0x80);   // mask the SHDN bit
}

void MAX30100_reset(void)
{
  uint8_t reg = MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG);  // Get the current register
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, reg | 0x40);   // mask the RESET bit
}

void MAX30100_clearFIFO(void)    //Clear FIFO read, write and overflow pointers to ensure FIFO is empty and in a known state
{
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_FIFO_WR_PTR, 0x00);
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_OVRFLOW_CTR, 0x00);
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_FIFO_RD_PTR, 0x00);
}

void MAX30100_startup(void)
{
  uint8_t reg = MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG);  // Get the current register
  MAX30100_I2CwriteByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG, reg & 0x7F);   // mask the SHDN bit
}

void MAX30100_printRegisters(void)
{
  Serial.print("Interrupt Status:   ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_INT_STATUS),  HEX);
  Serial.print("Interrupt Enable:   ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_INT_ENABLE),  BIN);
  Serial.print("FIFO Write Pointer: ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_FIFO_WR_PTR), BIN);
  Serial.print("Over Flow Counter:  ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_OVRFLOW_CTR), BIN);
  Serial.print("FIFO Read Pointer:  ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_FIFO_RD_PTR), BIN);
  Serial.print("FIFO Data register: ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_FIFO_DATA),   BIN);
  Serial.print("Mode Config:        ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG), BIN);
  Serial.print("SPO2 Config:        ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG), BIN);
  Serial.print("LED Config:         ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_LED_CONFIG),  BIN);
  Serial.print("Temp Int:           ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_TEMP_INTG),   BIN);
  Serial.print("Temp Fraction:      ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_TEMP_FRAC),   BIN);
  Serial.print("Revision ID:        ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_REV_ID),      BIN);
  Serial.print("Part ID:            ");
  Serial.println(MAX30100_I2CreadByte(MAX30100_ADDRESS, MAX30100_PART_ID),     BIN);
}



// Wire.h read and write protocols
void MAX30100_I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);  // Initialize the Tx buffer
  Wire.write(subAddress);           // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  Wire.endTransmission();           // Send the Tx buffer
}

uint8_t MAX30100_I2CreadByte(uint8_t address, uint8_t subAddress)
{
  
  uint8_t data; // `data` will store the register data
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);	                 // Put slave register address in Tx buffer
  Wire.endTransmission();                  // Send the Tx buffer, but send a restart to keep connection alive

  Wire.beginTransmission(address);
  Wire.requestFrom((uint8_t)address, (uint8_t)1);   // Read one byte from slave register address
  data = Wire.read();                               // Fill Rx buffer with result
  Wire.endTransmission();
  
  return data;                                      // Return data read from slave register
}

void MAX30100_I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count)
{
  //Serial.println("MAX30100_I2CreadBytes entered");
  Wire.beginTransmission(address);   // Initialize the Tx buffer
  //Serial.println("beginTransmission done");
  // Next send the register to be read. OR with 0x80 to indicate multi-read.
  Wire.write(subAddress);     // Put slave register address in Tx buffer
  //Serial.println("write done");
  Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
  //Serial.println("endTransmission done");
  uint8_t i = 0;
  Wire.requestFrom(address, count);  // Read bytes from slave register address
  while (Wire.available())
  {
    dest[i++] = Wire.read(); // Put read results in the Rx buffer
  }
}




//void setSPO2(uint8_t sr)--------------------------------------------------------------------Redundant
//{
//  uint8_t reg = I2CreadByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG);
//  reg = reg & 0xE3; // Set SPO2_SR to 000
//  I2CwriteByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, reg | (sr << 2)); // Mask SPO2_SR
//  reg = I2CreadByte(MAX30100_ADDRESS, MAX30100_MODE_CONFIG);
//  reg = reg & 0xf8; // Set Mode to 000
//  I2CwriteByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, reg | 0x03); // Mask MODE
//}----------------------------------------------------------------------------------------------------

//void setLEDs(uint8_t pw, uint8_t red, uint8_t ir)-------------------------------------------Redundant
//{
//  uint8_t reg = I2CreadByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG);
//  reg = reg & 0xFC; // Set LED_PW to 00
//  I2CwriteByte(MAX30100_ADDRESS, MAX30100_SPO2_CONFIG, reg | pw);     // Mask LED_PW
//  I2CwriteByte(MAX30100_ADDRESS, MAX30100_LED_CONFIG, (red << 4) | ir); // write LED configs
//}----------------------------------------------------------------------------------------------------


//int getRevID(void)---------------------------------------------------------------------------Not Used
//{
//  return I2CreadByte(MAX30100_ADDRESS, MAX30100_REV_ID);
//}
//
//int getPartID(void)
//{
//  return I2CreadByte(MAX30100_ADDRESS, MAX30100_PART_ID);
//}----------------------------------------------------------------------------------------------------
