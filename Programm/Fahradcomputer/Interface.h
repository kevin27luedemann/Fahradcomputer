/* 
* Interface.h
*
* Created: 02.09.2015 14:06:08
* Author: kevin
*/


#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define F_CPU 8000000

#include <avr/io.h>
#include "stdlib.h"
#include <util/delay.h>

class Interface
{
//variables
public:
protected:
private:

//functions
public:
	Interface();
	~Interface();
	uint8_t Taste(uint8_t wahl);
	uint8_t Taster();
	uint8_t ZahlenausTastatur();
protected:
private:
	Interface( const Interface &c );
	Interface& operator=( const Interface &c );
	uint8_t debounce(volatile uint8_t *port, uint8_t pin);

}; //Interface

#endif //__INTERFACE_H__
