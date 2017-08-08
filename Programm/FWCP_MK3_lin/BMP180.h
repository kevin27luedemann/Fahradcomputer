/* 
* BMP180.h
*
* Created: 02.02.2016 18:05:09
* Author: Lüdemann
*/


#ifndef __BMP180_H__
#define __BMP180_H__

#define BMP180_ADDR (0x77<<1) //0x77 default I2C address

//registers
#define BMP180_RAC1 0xAA
#define BMP180_RAC2 0xAC
#define BMP180_RAC3 0xAE
#define BMP180_RAC4 0xB0
#define BMP180_RAC5 0xB2
#define BMP180_RAC6 0xB4
#define BMP180_RB1 0xB6
#define BMP180_RB2 0xB8
#define BMP180_RMB 0xBA
#define BMP180_RMC 0xBC
#define BMP180_RMD 0xBE
#define BMP180_REGCONTROL 0xF4 //control
#define BMP180_REGCONTROLOUTPUT 0xF6 //output 0xF6=MSB, 0xF7=LSB, 0xF8=XLSB
#define BMP180_REGREADTEMPERATURE 0x2E //read temperature
#define BMP180_REGREADPRESSURE 0x34 //read pressure

//modes
#define BMP180_MODEULTRALOWPOWER 0 //oversampling=0, internalsamples=1, maxconvtimepressure=4.5ms, avgcurrent=3uA, RMSnoise_hPA=0.06, RMSnoise_m=0.5
#define BMP180_MODESTANDARD 1 //oversampling=1, internalsamples=2, maxconvtimepressure=7.5ms, avgcurrent=5uA, RMSnoise_hPA=0.05, RMSnoise_m=0.4
#define BMP180_MODEHIGHRES 2 //oversampling=2, internalsamples=4, maxconvtimepressure=13.5ms, avgcurrent=7uA, RMSnoise_hPA=0.04, RMSnoise_m=0.3
#define BMP180_MODEULTRAHIGHRES 3 //oversampling=3, internalsamples=8, maxconvtimepressure=25.5ms, avgcurrent=12uA, RMSnoise_hPA=0.03, RMSnoise_m=0.25

//setup parameters
#define BMP180_MODE BMP180_MODEULTRAHIGHRES //define a mode
#define BMP180_UNITPAOFFSET 0 //define a unit offset (pa)
#define BMP180_UNITMOFFSET 0 //define a unit offset (m)

#ifndef F_CPU
	#define F_CPU 8000000
#endif

#define PRESSURE00 1013.25

#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <util/delay.h>
#include "I2C.h"

class BMP180
{
//variables
public:
	double pressure;
	double pressure0;
	double temperature;
	double altitude;
	int16_t bmp180_regac1, bmp180_regac2, bmp180_regac3, bmp180_regb1, bmp180_regb2, bmp180_regmb, bmp180_regmc, bmp180_regmd;
	uint16_t bmp180_regac4, bmp180_regac5, bmp180_regac6;
	int32_t bmp180_rawtemperature, bmp180_rawpressure;
protected:
private:
	I2C i2c;
	
//functions
public:
	BMP180();
	~BMP180();
	//functions
	void bmp180_getcalibration();
	void bmp180_getpressure();
	void bmp180_getaltitude();
	void bmp180_gettemperature();
protected:
private:
	BMP180( const BMP180 &c );
	BMP180& operator=( const BMP180 &c );
	void bmp180_writemem(uint8_t reg, uint8_t value);
	int8_t readS8(uint8_t reg);
	int16_t readS16(uint8_t reg);
	uint8_t readU8(uint8_t reg);
	uint16_t readU16(uint8_t reg);
	uint32_t readU32(uint8_t reg);
	
}; //BMP180

#endif //__BMP180_H__
