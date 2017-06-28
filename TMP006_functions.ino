

// Configures sensor, use before reading from it
void TMP006_begin(uint16_t samples)
{
  Wire.begin();
  TMP006_write16(TMP006_ADDRESS, TMP006_CONFIG, samples | TMP006_CFG_MODEON);
}

// Read raw sensor temperature
int16_t TMP006_readRawDieTemperature()
{
  int16_t raw = TMP006_read16(TMP006_ADDRESS, TMP006_TAMB);

  raw >>= 2; //bitshift twice to get the 14-bit value from the 16-bit regerster
  return raw;
}

// Read raw thermopile voltage
int16_t TMP006_readRawVoltage() 
{
  int16_t raw = TMP006_read16(TMP006_ADDRESS, TMP006_VOBJ);

//  //10 value moving average--------------------
//  sumVoltage -= meanVoltage[9];
//  for (int i = 9; i > 0; i--)
//  {
//    meanVoltage[i] = meanVoltage[i - 1];
//  }
//  meanVoltage[0] = raw;
//  sumVoltage += raw;
//  float mean = sumVoltage / 10;
//  //-------------------------------------------
  
  return raw;
}

// Calculate object temperature based on raw sensor temp and thermopile voltage
double TMP006_calcObjTempC() 
{
  double Tdie = TMP006_readRawDieTemperature();
  //Serial.print("\nTdie = "); Serial.println(Tdie);
  double Vobj = TMP006_readRawVoltage();
  //Serial.print("Vobj = "); Serial.println(Vobj);
  
  Vobj *= 156.25;  // 156.25 nV per LSB
  Vobj /= 1000000000; // nV -> V
  Tdie *= 0.03125; // convert to celsius
  Tdie += 273.15; // convert to kelvin

  // Equations for calculating temperature found in section 5.1 in the user guide
  double tdie_tref = Tdie - TMP006_TREF;
  double S = (1 + TMP006_A1*tdie_tref + TMP006_A2*tdie_tref*tdie_tref);
  S *= TMP006_S0;
  S /= 10000000;
  S /= 10000000;

  double Vos = TMP006_B0 + TMP006_B1*tdie_tref + TMP006_B2*tdie_tref*tdie_tref;

  double fVobj = (Vobj - Vos) + TMP006_C2*(Vobj-Vos)*(Vobj-Vos);

  double Tobj = sqrt(sqrt(Tdie * Tdie * Tdie * Tdie + fVobj/S));

  Tobj -= 273.15; // Kelvin -> *C
  return Tobj;
}

// Caculate sensor temperature based on raw reading
double TMP006_readDieTempC() 
{
  double Tdie = TMP006_readRawDieTemperature();
  Tdie *= 0.03125; // convert to celsius
  return Tdie;
}


//----------------------I2C Functions---------------------------



// Read 16 bit int from I2C address addr and register reg
uint16_t TMP006_read16(uint8_t addr, uint8_t reg)
{
  uint16_t data;

  Wire.beginTransmission(addr);
  Wire.write(reg); // send register address to read from
  Wire.endTransmission();
   
  Wire.beginTransmission(addr);
  Wire.requestFrom((uint8_t)addr, (uint8_t)2); // request 2 bytes of data
  data = Wire.read(); // receive data
  data <<= 8;
  data |= Wire.read();
  Wire.endTransmission();

  return data;
}

// Write data to I2C address addr, register reg
void TMP006_write16(uint8_t addr, uint8_t reg, uint16_t data)
{
  Wire.beginTransmission(addr);
  Wire.write(reg); // sends register address to write to
  Wire.write(data>>8);  // write data
  Wire.write(data); 
  Wire.endTransmission();
}
