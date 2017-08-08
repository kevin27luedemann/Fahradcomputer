/* 
* Interface.h
*
* Created: 02.09.2015 14:06:08
* Author: kevin
*/


#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

class Interface
{
//variables
public:
protected:
private:
uint8_t stat[12];

//functions
public:
	Interface();
	~Interface();
	uint8_t Taste(uint8_t wahl);
	uint8_t Taster();
	//new unified Layout similar to the GameBoy
	uint8_t unified();
	uint8_t ZahlenausTastatur();
protected:
private:
	Interface( const Interface &c );
	Interface& operator=( const Interface &c );
	uint8_t debounce(volatile uint8_t *port, uint8_t pin);
	uint8_t paradeb(volatile uint8_t *port, uint8_t pin, uint8_t *tasterstat);

}; //Interface

#endif //__INTERFACE_H__
