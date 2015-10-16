/* 
* SOUND.cpp
*
* Created: 15.10.2015 19:25:11
* Author: kevin
*/


#include "SOUND.h"

// default constructor
SOUND::SOUND()
{
	//interuptregister initialisieren
	interupts = 0;
	PORTB &= ~(1<<PORTB3); //Pullups ausschalten
	DDRB |= (1<<PORTB3); //Ausgang schalten
	//Not inverting PWM, Phase correct
	TCCR0A |= (1<<COM0A0) | (1<<WGM00);
	TCCR0B |= (1<<WGM02);
	//Fuer 1024 Prescaler um von 14KHz bis 60 Hz zu gehen
	//nach Formel f=f_CPU/(PRESCALER*OCR0A)
/*	f		bei		1			256
	fuer	8		1MHz		3.90625KHz
			64		125KHz		488.28Hz
			256		31.25KHz	122.07Hz
			1024	7.8125KHz	30.52Hz
*/
	prescaler = (1<<CS02); //256
	setfrequenci(1000);//1KHz
	off();
} //SOUND

// default destructor
SOUND::~SOUND()
{
} //~SOUND


//WERT = f_CPU/(f*1024)
void SOUND::setfrequenci(uint16_t frequenz){
	off();
	float WERT = F_CPU/256.0;
	WERT /= (frequenz);
	OCR0A = (uint8_t)WERT;
	TCNT0 = 0;
	on();
}

void SOUND::on(){
	if ((interupts&(1<<Soundactiv)))
	{
		TCCR0B |= prescaler;
	}
}

void SOUND::off(){
	TCCR0B &= ~((1<<CS00)|(1<<CS01)|(1<<CS02));
}
