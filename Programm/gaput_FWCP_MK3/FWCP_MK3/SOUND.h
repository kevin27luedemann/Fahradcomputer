/* 
* SOUND.h
*
* Created: 15.10.2015 19:25:11
* Author: kevin
*/


#ifndef __SOUND_H__
#define __SOUND_H__

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define Soundactiv 0

class SOUND
{
//variables
public:
protected:
private:
	uint8_t interupts;
	uint8_t prescaler;

//functions
public:
	SOUND();
	~SOUND();
	void setfrequenci(uint16_t frequenz);
	void on();
	void off();
protected:
private:
	SOUND( const SOUND &c );
	SOUND& operator=( const SOUND &c );

}; //SOUND

#endif //__SOUND_H__
