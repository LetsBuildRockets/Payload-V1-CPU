/***************************************************************************
  This is a library for the LSM303 Accelerometer and magnentometer/compass

  Designed specifically to work with the Adafruit LSM303DLHC Breakout

  These displays use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include "Adafruit_LSM303_U_SOFT.h"
SoftI2CMaster i2c = SoftI2CMaster();
/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
bool Adafruit_LSM303::begin()
{
  i2c.setPins(5,3,true);
Serial.println("i2c");

  // Enable the accelerometer
  write8(LSM303_ADDRESS_ACCEL, LSM303_REGISTER_ACCEL_CTRL_REG1_A, 0x27);
  
  // Enable the magnetometer
  write8(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_MR_REG_M, 0x00);

  return true;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
void Adafruit_LSM303::read()
{
  // Read the accelerometer
  i2c.beginTransmission((byte)LSM303_ADDRESS_ACCEL);
  i2c.write(LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80);
  i2c.endTransmission();
  i2c.requestFrom((byte)LSM303_ADDRESS_ACCEL/*, (byte)6*/);

  // Wait around until enough data is available
  //while (i2c.available() < 6);

  uint8_t xlo = i2c.read();
  uint8_t xhi = i2c.read();
  uint8_t ylo = i2c.read();
  uint8_t yhi = i2c.read();
  uint8_t zlo = i2c.read();
  uint8_t zhi = i2c.read();

  // Shift values to create properly formed integer (low byte first)
  accelData.x = (xlo | (xhi << 8)) >> 4;
  accelData.y = (ylo | (yhi << 8)) >> 4;
  accelData.z = (zlo | (zhi << 8)) >> 4;
  
  // Read the magnetometer
  i2c.beginTransmission((byte)LSM303_ADDRESS_MAG);
  i2c.write(LSM303_REGISTER_MAG_OUT_X_H_M);
  i2c.endTransmission();
  i2c.requestFrom((byte)LSM303_ADDRESS_MAG/*, (byte)6*/);
  
  // Wait around until enough data is available
  //while (i2c.available() < 6);

  // Note high before low (different than accel)  
  xhi = i2c.read();
  xlo = i2c.read();
  zhi = i2c.read();
  zlo = i2c.read();
  yhi = i2c.read();
  ylo = i2c.read();
  
  // Shift values to create properly formed integer (low byte first)
  magData.x = (xlo | (xhi << 8));
  magData.y = (ylo | (yhi << 8));
  magData.z = (zlo | (zhi << 8));  
  
  // ToDo: Calculate orientation
  magData.orientation = 0.0;
}

void Adafruit_LSM303::setMagGain(lsm303MagGain gain)
{
  write8(LSM303_ADDRESS_MAG, LSM303_REGISTER_MAG_CRB_REG_M, (byte)gain);
}

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/
void Adafruit_LSM303::write8(byte address, byte reg, byte value)
{
  i2c.beginTransmission(address);
  i2c.write(reg);
  i2c.write(value);
  i2c.endTransmission();
}

byte Adafruit_LSM303::read8(byte address, byte reg)
{
  byte value;

  i2c.beginTransmission(address);
  i2c.write(reg);
  i2c.endTransmission();
  i2c.requestFrom(address/*, (byte)1*/);
  value = i2c.read();
  i2c.endTransmission();

  return value;
}

