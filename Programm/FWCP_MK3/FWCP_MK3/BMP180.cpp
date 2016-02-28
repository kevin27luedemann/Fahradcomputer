/* 
* BMP180.cpp
*
* Created: 02.02.2016 18:05:08
* Author: Lüdemann
*/


#include "BMP180.h"

// default constructor
BMP180::BMP180()
{
	bmp180_rawtemperature = 0;
	bmp180_rawpressure = 0;
	pressure0 = PRESSURE00;
	
	bmp180_getcalibration();
	
} //BMP180

// default destructor
BMP180::~BMP180()
{
} //~BMP180bmp180_regac2

void BMP180::bmp180_writemem(uint8_t reg, uint8_t value){
	i2c.twi_start();
	i2c.twi_write(BMP180_ADDR | I2C_WRITE);
	i2c.twi_write(reg);
	i2c.twi_write(value);
	i2c.twi_stop();
}

int8_t BMP180::readS8(uint8_t reg){
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_WRITE));
	i2c.twi_write(reg);
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_READ));
	int8_t ergeb = i2c.twi_read(0);
	i2c.twi_stop();
	return ergeb;
}
int16_t BMP180::readS16(uint8_t reg){
	int8_t	hi = readS8(reg);
	uint8_t	lo = readU8(reg+1);
	return ( hi << 8) + lo;
}
uint8_t BMP180::readU8(uint8_t reg){
		i2c.twi_start();
		i2c.twi_write((BMP180_ADDR | I2C_WRITE));
		i2c.twi_write(reg);
		i2c.twi_start();
		i2c.twi_write((BMP180_ADDR | I2C_READ));
		uint8_t ergeb = i2c.twi_read(0);
		i2c.twi_stop();
		return ergeb;
}
uint16_t BMP180::readU16(uint8_t reg){
	uint8_t	hi = readU8(reg);
	uint8_t	lo = readU8(reg+1);
	return (hi << 8) + lo;
}
uint32_t BMP180::readU32(uint8_t reg){
	uint8_t hix	= readS8(reg);
	uint8_t hi	= readU8(reg+1);
	uint8_t lo	= readU8(reg+2);
	return (((int32_t) hix << 16) + ((uint32_t) hi << 8) +(uint32_t) lo);
}

void BMP180::bmp180_getcalibration() {
	bmp180_regac1	= readS16(BMP180_RAC1);
	bmp180_regac2	= readS16(BMP180_RAC2);
	bmp180_regac3	= readS16(BMP180_RAC3);
	bmp180_regac4	= readU16(BMP180_RAC4);
	bmp180_regac5	= readU16(BMP180_RAC5);
	bmp180_regac6	= readU16(BMP180_RAC6);
	bmp180_regb1	= readS16(BMP180_RB1);
	bmp180_regb2	= readS16(BMP180_RB2);
	bmp180_regmb	= readS16(BMP180_RMB);
	bmp180_regmc	= readS16(BMP180_RMC);
	bmp180_regmd	= readS16(BMP180_RMD);
	/*
	bmp180_regac1	= 7843;
	bmp180_regac2	= -1109;
	bmp180_regac3	= -14369;
	bmp180_regac4	= 33802;
	bmp180_regac5	= 25803;
	bmp180_regac6	= 18752;
	bmp180_regb1	= 6515;
	bmp180_regb2	= 41;
	bmp180_regmb	= -32768;
	bmp180_regmc	= -11786;
	bmp180_regmd	= 2331;
	*/
}

void BMP180::bmp180_getpressure(){
	int64_t up;
	int64_t x1,x2,x3,b3,b6,p;
	uint64_t b4,b7;

	bmp180_gettemperature();

	//read raw pressure
	bmp180_writemem(BMP180_REGCONTROL, BMP180_REGREADPRESSURE+(BMP180_MODE << 6));
	_delay_ms(2 + (3<<BMP180_MODE));
	
	up   = readU32(BMP180_REGCONTROLOUTPUT);
	up >>= (8-BMP180_MODE);

	//calculate raw pressure
	b6   = bmp180_rawtemperature - 4000;
	x1   = b6*b6;
	x1 >>= 12;
	x1  *= bmp180_regb2;
	x1 >>= 11;
	x2   = bmp180_regac2 * b6;
	x2 >>= 11;
	x3   = x1 + x2;
	b3   = bmp180_regac1 * 4 +x3;
	b3 <<= BMP180_MODE;
	b3  += 2;
	b3  >>= 2;
	x1   = bmp180_regac3 * b6;
	x1 >>= 13;
	x2   = b6*b6;
	x2 >>= 12;
	x2  *= bmp180_regb1;
	x2 >>= 16;
	x3   = x1 + x2;
	x3  += 2;
	x3 >>= 2;
	b4   = (uint64_t)(x3+32768);
	b4  *= bmp180_regac4;
	b4 >>= 15;
	b7   = ((uint64_t)up - b3);
	b7  *= (50000 >> BMP180_MODE);
	if (b7 < 0x80000000){
		p  = b7*2;
		p /= b4;
	}
	else {
		p  = b7/b4;
		p *= p;
	}
	x1   = (p >> 8);
	x1  *= (p >> 8);
	x1   = (x1 * 3038);
	x1 >>= 16;
	x2   = (-7357 * p);
	x2 >>= 16;
	bmp180_rawpressure   = (x1 + x2 + 3791);
	bmp180_rawpressure >>= 4;
	bmp180_rawpressure  += p;
	//Korrektur heuristisch 
	//bmp180_rawpressure  -= 24000;
	
	pressure = (bmp180_rawpressure+BMP180_UNITPAOFFSET)/100.0;
	
}

void BMP180::bmp180_getaltitude(){
	bmp180_getpressure();
	altitude  = pressure/pressure0;
	altitude  = pow(altitude,1/5.255);
	altitude  = 1-altitude;
	altitude *= 44330;
}

void BMP180::bmp180_gettemperature(){
	
	int16_t ut;
	int64_t x1,x2;

	//read raw temperature
	bmp180_writemem(BMP180_REGCONTROL, BMP180_REGREADTEMPERATURE);
	_delay_ms(5); // min. 4.5ms read Temp delay
	//bmp180_readmem(BMP180_REGCONTROLOUTPUT, buff, 2);
	ut  = readS16(BMP180_REGCONTROLOUTPUT);

	//calculate raw temperature
	x1	  = ( ut - bmp180_regac6);
	x1	 *= bmp180_regac5;
	x1	>>= 15;
	x2	  = bmp180_regmc;
	x2	<<= 11;
	x2	 /= (x1 + bmp180_regmd);
	bmp180_rawtemperature = x1 + x2;
	
	//bmp180_rawtemperature = 4008;
	temperature	 = (bmp180_rawtemperature+8);
	temperature	/= 16.0;
	temperature	/= 10.0;
	
}

