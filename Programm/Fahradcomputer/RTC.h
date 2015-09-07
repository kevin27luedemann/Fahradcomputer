/* 
* RTC.h
*
* Created: 02.09.2015 12:17:52
* Author: kevin
*/


#ifndef __RTC_H__
#define __RTC_H__

//Konstanten
//#define TIMER2RTCTIME 112
#define TIMER2RTCTIME 0
#define bitsderrtc 8

//interuptbits
#define hundinterupt 0
#define sekundeninterupt 1
#define minuteninterupt 2
//Stoppuhrbits
#define stoppbit 2

#include <avr/io.h>
#include <stdlib.h>

class RTC
{
//variables
public:
	char msg_uhr[bitsderrtc];
	char msg_dat[bitsderrtc];
	uint8_t Stunden;
	uint8_t Minuten;
	uint8_t Sekunden;
	uint8_t HundSekunden;
	uint8_t Tag;
	uint8_t Monat;
	uint8_t Jahr; //Jahr=Jahr ab 2000 als integer
	uint8_t interupts;	//Flagregister, platz fuer 8 Flags
protected:
private:

//functions
public:
	RTC();
	~RTC();
	uint8_t zeit();
	uint8_t zeit_hund();
	void dummyeinst();
	void RTCstart();
	void RTCstop();
	uint8_t timer();
	void ausgabezeitneu();
	void ausgabedatumneu();
protected:
private:
	RTC( const RTC &c );
	RTC& operator=( const RTC &c );
	void kalender();

}; //RTC

#endif //__RTC_H__
