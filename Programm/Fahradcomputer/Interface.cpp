/* 
* Interface.cpp
*
* Created: 02.09.2015 14:06:08
* Author: kevin
*/


#include "Interface.h"

// default constructor
Interface::Interface()
{
} //Interface

// default destructor
Interface::~Interface()
{
} //~Interface

uint8_t Interface::debounce(volatile uint8_t *port, uint8_t pin)
{
	if ( (*port & (1 << pin)) )
	{
		/* Pin wurde auf Masse gezogen, 100ms warten   */
		_delay_ms(1);
		if ( !(*port & (1 << pin)) )
		{
			/* Anwender Zeit zum Loslassen des Tasters geben */
			_delay_us(10);
			return 1;
		}
	}
	return 0;
}

uint8_t Interface::Taste(uint8_t wahl){
	uint8_t abfrage=0;
	switch (wahl)
	{
		case '1':
		PORTD |= (1<<PIND3);
		_delay_us(1);
		if (debounce(&PIND,PIND6))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD3);
		break;
		case '2':
		PORTD |= (1<<PIND3);
		_delay_us(1);
		if (debounce(&PIND,PIND5))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD3);
		break;
		case '3':
		PORTD |= (1<<PIND3);
		_delay_us(1);
		if (debounce(&PIND,PIND4))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD3);
		break;
		case '4':
		PORTD |= (1<<PIND2);
		_delay_us(1);
		if (debounce(&PIND,PIND6))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD2);
		break;
		case '5':
		PORTD |= (1<<PIND2);
		_delay_us(1);
		if (debounce(&PIND,PIND5))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD2);
		break;
		case '6':
		PORTD |= (1<<PIND2);
		_delay_us(1);
		if (debounce(&PIND,PIND4))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD2);
		break;
		case '7':
		PORTD |= (1<<PIND1);
		_delay_us(1);
		if (debounce(&PIND,PIND6))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD1);
		break;
		case '8':
		PORTD |= (1<<PIND1);
		_delay_us(1);
		if (debounce(&PIND,PIND5))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD1);
		break;
		case '9':
		PORTD |= (1<<PIND1);
		_delay_us(1);
		if (debounce(&PIND,PIND4))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD1);
		break;
		case '*':
		PORTD |= (1<<PIND0);
		_delay_us(1);
		if (debounce(&PIND,PIND6))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD0);
		break;
		case '0':
		PORTD |= (1<<PIND0);
		_delay_us(1);
		if (debounce(&PIND,PIND5))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD0);
		break;
		case '#':
		PORTD |= (1<<PIND3);
		_delay_us(1);
		if (debounce(&PIND,PIND4))
		{
			abfrage = 1;
		}
		PORTD &= ~(1<<PORTD0);
		break;
		default:
		abfrage = 0;
		break;
	}
	return abfrage;
}

uint8_t Interface::Taster(){
	uint8_t taste = 'A';
	
	//erste Zeile
	PORTD |= (1<<PORTD3);
	_delay_us(1);
	if(debounce(&PIND,PIND4)){
		taste = '3';
	}
	else if (debounce(&PIND,PIND5))
	{
		taste = '2';
	}
	else if (debounce(&PIND,PIND6))
	{
		taste = '1';
	}
	PORTD &= ~(1<<PORTD3);
	
	//zweite Zeile
	PORTD |= (1<<PORTD2);
	_delay_us(1);
	if(debounce(&PIND,PIND4)){
		taste = '6';
	}
	else if (debounce(&PIND,PIND5))
	{
		taste = '5';
	}
	else if (debounce(&PIND,PIND6))
	{
		taste = '4';
	}
	PORTD &= ~(1<<PORTD2);
	
	//dritte Zeile
	PORTD |= (1<<PORTD1);
	_delay_us(1);
	if(debounce(&PIND,PIND4)){
		taste = '9';
	}
	else if (debounce(&PIND,PIND5))
	{
		taste = '8';
	}
	else if (debounce(&PIND,PIND6))
	{
		taste = '7';
	}
	PORTD &= ~(1<<PORT1);
	
	//vierte Zeile
	PORTD |= (1<<PORTD0);
	_delay_us(1);
	if(debounce(&PIND,PIND4)){
		taste = '#';
	}
	else if (debounce(&PIND,PIND5))
	{
		taste = '0';
	}
	else if (debounce(&PIND,PIND6))
	{
		taste = '*';
	}
	PORTD &= ~(1<<PORTD0);
	
	return taste;
}

uint8_t Interface::ZahlenausTastatur(){
	while(true){
		uint8_t taste = Taster();
		if(taste>='0' && taste<='9'){
			return taste - '0';
		}
	}
}
