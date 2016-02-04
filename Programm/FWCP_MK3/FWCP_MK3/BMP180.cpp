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

void BMP180::bmp180_readmem(uint8_t reg, uint8_t buff[], uint8_t bytes){
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_WRITE));
	i2c.twi_write(reg);
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_READ));
	
	for(uint8_t i=0; i<bytes;i++){
		if (i<(bytes-1))
		{
			buff[i]=i2c.twi_read(1);
		}
		else
		{
			buff[i]=i2c.twi_read(0);
		}
	}
	i2c.twi_stop();
}

void BMP180::bmp180_getcalibration() {
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_WRITE));
	i2c.twi_write(BMP180_REGAC1);
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_READ));
	bmp180_regac1 = (i2c.twi_read(1) << 8);
	bmp180_regac1 += (i2c.twi_read(1));
	
	bmp180_regac2 = (i2c.twi_read(1) << 8);
	bmp180_regac2 += (i2c.twi_read(1));
	
	bmp180_regac3 = (i2c.twi_read(1) << 8);
	bmp180_regac3 += (i2c.twi_read(1));
	
	bmp180_regac4 = (i2c.twi_read(1) << 8);
	bmp180_regac4 += (i2c.twi_read(1));
	
	bmp180_regac5 = (i2c.twi_read(1) << 8);
	bmp180_regac5 += (i2c.twi_read(1));
	
	bmp180_regac6 = (i2c.twi_read(1) << 8);
	bmp180_regac6 += (i2c.twi_read(1));
	
	bmp180_regb1 = (i2c.twi_read(1) << 8);
	bmp180_regb1 += (i2c.twi_read(1));
	
	bmp180_regb2 = (i2c.twi_read(1) << 8);
	bmp180_regb2 += (i2c.twi_read(1));
	
	bmp180_regmb = (i2c.twi_read(1) << 8);
	bmp180_regmb += (i2c.twi_read(1));
	
	bmp180_regmc = (i2c.twi_read(1) << 8);
	bmp180_regmc += (i2c.twi_read(1));
	
	bmp180_regmd = (i2c.twi_read(1) << 8);
	bmp180_regmd += (i2c.twi_read(0));
	
	i2c.twi_stop();
}

void BMP180::bmp180_getpressure(){
	int32_t up,x1,x2,x3,b3,b6,p;
	uint32_t b4,b7;

	bmp180_gettemperature();

	//read raw pressure
	bmp180_writemem(BMP180_REGCONTROL, BMP180_REGREADPRESSURE+(BMP180_MODE << 6));
	_delay_ms(2 + (3<<BMP180_MODE));
	
	//bmp180_readmem(BMP180_REGCONTROLOUTPUT, buff, 3);
	//up = ((((int32_t)buff[0] <<16) | ((int32_t)buff[1] <<8) | ((int32_t)buff[2])) >> (8-BMP180_MODE)); // uncompensated pressure value
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_WRITE));
	i2c.twi_write(BMP180_REGCONTROLOUTPUT);
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_READ));
	up   = i2c.twi_read(1) << 16;
	up  += i2c.twi_read(1) << 8;
	up  += i2c.twi_read(0);
	up >>= (8-BMP180_MODE);
	i2c.twi_stop();

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
	b3  /= 4;
	x1   = bmp180_regac3 * b6;
	x1 >>= 13;
	x2   = b6*b6;
	x2 >>= 12;
	x2  *= bmp_regb1;
	x2 >>= 10;
	x3   = x1 + x2;
	x3  += 2;
	x3 >>= 2;
	b4   = (uint32_t)(x3+32768);
	b4  *= bmp180_regac4;
	b4 >>= 15;
	b7   = ((uint32_t)up - b3);
	b7  *= (50000 >> BMP180_MODE);
	if (b7 < 0x80000000){
		p  = b7*2;
		p /= b4;
	}
	else {
		p  = b7/b4;
		p *= 2;
	}
	x1   = (p >> 8);
	x1  *= (p >> 8);
	x1   = (x1 * 3038);
	x1 >>= 10;
	x2   = (-7357 * p);
	x2 >>= 16;
	bmp180_rawpressure   = (x1 + x2 + 3791);
	bmp180_rawpressure >>= 4;
	bmp180_rawpressure  += p;
	
	pressure = (bmp180_rawpressure+BMP180_UNITPAOFFSET)/100.0;
	
}

void BMP180::bmp180_getaltitude(){
	bmp180_getpressure();
	
}

void BMP180::bmp180_gettemperature(){
	//uint8_t buff[2] = {0,0};
	int32_t ut,x1,x2;

	//read raw temperature
	bmp180_writemem(BMP180_REGCONTROL, BMP180_REGREADTEMPERATURE);
	_delay_ms(5); // min. 4.5ms read Temp delay
	//bmp180_readmem(BMP180_REGCONTROLOUTPUT, buff, 2);
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_WRITE));
	i2c.twi_write(BMP180_REGCONTROLOUTPUT);
	i2c.twi_start();
	i2c.twi_write((BMP180_ADDR | I2C_READ));
	ut  = (i2c.twi_read(1)<<8);
	ut += i2c.twi_read(0);
	i2c.twi_stop();

	//calculate raw temperature
	x1	  = (ut - bmp180_regac6);
	x1	 *= bmp180_regac5;
	x1	>>= 15;
	x2	  = (bmp180_regmc << 11);
	x2	 /= (x1 + bmp180_regmd);
	bmp180_rawtemperature = x1 + x2;
	
	temperature	  = (bmp180_rawtemperature+8);
	temperature	>>= 4;
	temperature	 /= 10.0;
}

