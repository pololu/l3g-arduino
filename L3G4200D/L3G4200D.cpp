#include <L3G4200D.h>
#include <Wire.h>
#include <math.h>
#include <vector.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address, 
// and sets the last bit correctly based on reads and writes
#define GYR_ADDRESS (0xD2 >> 1)

// Public Methods //////////////////////////////////////////////////////////////

// Turns on the L3G4200D's gyro and places it in normal mode.
void L3G4200D::enableDefault(void)
{
	// 0x0F = 0b00001111
	// Normal power mode, all axes enabled
	writeReg(L3G4200D_CTRL_REG1, 0x0F);
}

// Writes a gyro register
void L3G4200D::writeReg(byte reg, byte value)
{
	Wire.beginTransmission(MAG_ADDRESS);
	Wire.send(reg);
	Wire.send(value);
	Wire.endTransmission();
}

// Reads a gyro register
byte L3G4200D::readReg(byte reg)
{
	byte value;
	
	Wire.beginTransmission(MAG_ADDRESS);
	Wire.send(reg);
	Wire.endTransmission();
	Wire.requestFrom(MAG_ADDRESS, 1);
	value = Wire.receive();
	Wire.endTransmission();
	
	return value;
}

// Reads the 3 gyro channels and stores them in vector g
void L3G4200D::read()
{
	Wire.beginTransmission(GYR_ADDRESS);
	// assert the MSB of the address to get the gyro 
	// to do slave-transmit subaddress updating.
	Wire.send(L3G4200D_OUT_X_L | (1 << 7)); 
	Wire.endTransmission();
	Wire.requestFrom(GYR_ADDRESS, 6);

	while (Wire.available() < 6);
	
	uint8_t xla = Wire.receive();
	uint8_t xha = Wire.receive();
	uint8_t yla = Wire.receive();
	uint8_t yha = Wire.receive();
	uint8_t zla = Wire.receive();
	uint8_t zha = Wire.receive();

	g.x = (xha << 8 | xla) >> 4;
	g.y = (yha << 8 | yla) >> 4;
	g.z = (zha << 8 | zla) >> 4;
}