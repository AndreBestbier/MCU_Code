
// Sensor address
#define TMP006_ADDRESS        0x40  // 8bit address converted to 7bit

// Constants for calculating object temperature
#define TMP006_B0 -0.0000294
#define TMP006_B1 -0.00000057
#define TMP006_B2 0.00000000463
#define TMP006_C2 13.4
#define TMP006_TREF 298.15
#define TMP006_A1 0.00175
#define TMP006_A2 -0.00001678
#define TMP006_S0 9.4  // * 10^-14  (originally 6.4)

// Configuration Settings
#define TMP006_CFG_RESET    0x8000
#define TMP006_CFG_MODEON   0x7000

#define TMP006_CFG_1SAMPLE  0x0000
#define TMP006_CFG_2SAMPLE  0x0200
#define TMP006_CFG_4SAMPLE  0x0400
#define TMP006_CFG_8SAMPLE  0x0600
#define TMP006_CFG_16SAMPLE 0x0800

#define TMP006_CFG_DRDYEN   0x0100
#define TMP006_CFG_DRDY     0x0080

// Registers to read thermopile voltage and sensor temperature
#define TMP006_VOBJ  0x00
#define TMP006_TAMB 0x01
#define TMP006_CONFIG 0x02


// Configures sensor, use before reading from it
void TMP006_begin(uint16_t samples);

// Read raw sensor temperature
int16_t TMP006_readRawDieTemperature();

// Read raw thermopile voltage
int16_t TMP006_readRawVoltage();

// Calculate object temperature based on raw sensor temp and thermopile voltage
double TMP006_calcObjTempC();

// Caculate sensor temperature based on raw reading
double TMP006_readDieTempC();

//--------------------I2C functions-----------------


// Read 16 bit int from I2C address addr and register reg
uint16_t TMP006_read16(uint8_t addr, uint8_t reg);

// Write data to I2C address addr, register reg
void TMP006_write16(uint8_t addr, uint8_t reg, uint16_t data);
