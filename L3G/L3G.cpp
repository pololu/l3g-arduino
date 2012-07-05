#include <L3G4200D.h>
#include <Wire.h>
#include <math.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address, 
// and sets the last bit correctly based on reads and writes
#define L3G4200D_ADDRESS_SDO_LOW  (0xD0 >> 1)
#define L3G4200D_ADDRESS_SDO_HIGH (0xD2 >> 1)
#define L3GD20_ADDRESS_SDO_LOW    (0xD4 >> 1)
#define L3GD20_ADDRESS_SDO_HIGH   (0xD6 >> 1)

// Public Methods //////////////////////////////////////////////////////////////

bool L3G4200D::init(byte device, byte sdo)
{	
	_device = device;
	switch (_device)
	{
		case L3G4200D_DEVICE:
			if (sdo == L3G_SDO_LOW)
			{
				address = L3G4200D_ADDRESS_SDO_LOW;
				return true;
			}
			else if (sdo == L3G_SDO_HIGH)
			{
				address = L3G4200D_ADDRESS_SDO_HIGH;
				return true;
			}
			else
				return autoDetectAddress();
			break;	
		
		case L3GD20_DEVICE:
			if (sdo == L3G_SDO_LOW)
			{
				address = L3GD20_ADDRESS_SDO_LOW;
				return true;
			}
			else if (sdo == L3G_SDO_HIGH)
			{
				address = L3GD20_ADDRESS_SDO_HIGH;
				return true;
			}
			else
				return autoDetectAddress();
			break;	
			
		default:
			return autoDetectAddress();
	}
}

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
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

// Reads a gyro register
byte L3G4200D::readReg(byte reg)
{
	byte value;
	
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(address, (byte)1);
	value = Wire.read();
	Wire.endTransmission();
	
	return value;
}

// Reads the 3 gyro channels and stores them in vector g
void L3G4200D::read()
{
	Wire.beginTransmission(address);
	// assert the MSB of the address to get the gyro 
	// to do slave-transmit subaddress updating.
	Wire.write(L3G4200D_OUT_X_L | (1 << 7)); 
	Wire.endTransmission();
	Wire.requestFrom(address, (byte)6);

	while (Wire.available() < 6);
	
	uint8_t xla = Wire.read();
	uint8_t xha = Wire.read();
	uint8_t yla = Wire.read();
	uint8_t yha = Wire.read();
	uint8_t zla = Wire.read();
	uint8_t zha = Wire.read();

	g.x = xha << 8 | xla;
	g.y = yha << 8 | yla;
	g.z = zha << 8 | zla;
}

void L3G4200D::vector_cross(const vector *a,const vector *b, vector *out)
{
  out->x = a->y*b->z - a->z*b->y;
  out->y = a->z*b->x - a->x*b->z;
  out->z = a->x*b->y - a->y*b->x;
}

float L3G4200D::vector_dot(const vector *a,const vector *b)
{
  return a->x*b->x+a->y*b->y+a->z*b->z;
}

void L3G4200D::vector_normalize(vector *a)
{
  float mag = sqrt(vector_dot(a,a));
  a->x /= mag;
  a->y /= mag;
  a->z /= mag;
}

// Private Methods //////////////////////////////////////////////////////////////

bool L3G4200D::autoDetectAddress(void)
{
	// try each possible address and stop if reading WHO_AM_I returns the expected response
	address = L3G4200D_ADDRESS_SDO_LOW;
	if (readReg(L3G4200D_WHO_AM_I) == 0xD3) return true;
	address = L3G4200D_ADDRESS_SDO_HIGH;
	if (readReg(L3G4200D_WHO_AM_I) == 0xD3) return true;
	address = L3GD20_ADDRESS_SDO_LOW;
	if (readReg(L3G4200D_WHO_AM_I) == 0xD4) return true;
	address = L3GD20_ADDRESS_SDO_HIGH;
	if (readReg(L3G4200D_WHO_AM_I) == 0xD4) return true;
	
	return false;
}
