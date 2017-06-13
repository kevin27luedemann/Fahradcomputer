/* 
* Kompass.h
*
* Created: 02.09.2015 11:47:38
* Author: kevin
*/


#ifndef __KOMPASS_H__
#define __KOMPASS_H__

//Konstanten
#define HMCWRITEADRESSE 0x3C
#define HMCREADADRESSE 0x3D
#define HMCSIGLEMODE 0b00000001
#define HMCCONTINUOUSMODE 0b00000000
#define HMCIDLEMODE 0b00000011
#define HMCGAIN1370 0b00000000
#define HMCGAIN1090 0b00100000
#define HMCGAIN820 0b01000000
#define HMCGAIN660 0b01100000
#define HMCGAIN440 0b10000000
#define HMCGAIN390 0b10100000
#define HMCGAIN330 0b11000000
#define HMCGAIN230 0b11100000
#define HMCSAMPLES1 0b00000000
#define HMCSAMPLES2 0b00100000
#define HMCSAMPLES4 0b01000000
#define HMCSAMPLES8 0b01100000
#define HMCDATARATE0075 0b00000000
#define HMCDATARATE105 0b00000100
#define HMCDATARATE3 0b00001000
#define HMCDATARATE705 0b00001100
#define HMCDATARATE15 0b00010000
#define HMCDATARATE30 0b00011000
#define HMCDATARATE75 0b00011000
#define HMCMEASUREMODENORMAL 0b00000000
#define HMCMEASUREMODEPOSITIVE 0b00000001
#define HMCMEASUREMODENEGATIVE 0b00000010

//includes
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <math.h>
#include "I2C.h"

class Kompass
{
//variables
public:
	int16_t achsen[3];
protected:
private:
	I2C i2c;
	int16_t offset[3];
	int16_t max[3];
	int16_t min[3];

//functions
public:
	Kompass();
	~Kompass();
	void HMC5883L_command(uint8_t adresse, uint8_t command);
	void initialize_HMC5883L();
	uint8_t HMC5883L_readHeading();
	double angle(double roll, double pitch);
	void kallibrierung_ruecksetzen();
protected:
private:
	Kompass( const Kompass &c );
	Kompass& operator=( const Kompass &c );

}; //Kompass

#endif //__KOMPASS_H__
