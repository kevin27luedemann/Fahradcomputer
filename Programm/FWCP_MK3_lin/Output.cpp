/* 
* Output.cpp
*
* Created: 09.09.2015 16:01:30
* Author: kevin
*/


#include "Output.h"

// default constructor
Output::Output(uint8_t Portpraefix, uint8_t Pinnumber)
{
	Pin=Pinnumber;
	Port=Portpraefix;
	switch (Portpraefix)
	{
		case 'A':
			DDRA |= (1<<Pin);
			break;
		case 'B':
			DDRB |= (1<<Pin);
			break;
		case 'C':
			DDRC |= (1<<Pin);
			break;
		case 'D':
			DDRD |= (1<<Pin);
			break;
		default:
			break;
	}
} //Output

// default destructor
Output::~Output()
{
} //~Output

void Output::on(){
	switch (Port)
	{
		case 'A':
			PORTA |= (1<<Pin);
			break;
		case 'B':
			PORTB |= (1<<Pin);
			break;
		case 'C':
			PORTC |= (1<<Pin);
			break;
		case 'D':
			PORTD |= (1<<Pin);
			break;
		default:
			break;
	}
}

uint8_t Output::ison(){
	uint8_t returnparam=0;
	switch (Port)
	{
		case 'A':
			if ((PORTA&(1<<Pin)))
			{
				returnparam = 1;
			}
			break;
		case 'B':
			if ((PORTB&(1<<Pin)))
			{
				returnparam = 1;
			}
			break;
		case 'C':
			if ((PORTC&(1<<Pin)))
			{
				returnparam = 1;
			}
			break;
		case 'D':
			if ((PORTD&(1<<Pin)))
			{
				returnparam = 1;
			}
			break;
		default:
		returnparam = 2;
			break;
	}
	return returnparam;
}

void Output::off(){
	switch (Port)
	{
		case 'A':
			PORTA &= ~(1<<Pin);
			break;
		case 'B':
			PORTB &= ~(1<<Pin);
			break;
		case 'C':
			PORTC &= ~(1<<Pin);
			break;
		case 'D':
			PORTD &= ~(1<<Pin);
			break;
		default:
			break;
	}
}

void Output::toggle(){
	switch (Port)
	{
		case 'A':
			if ((PORTA&(1<<Pin)))
			{
				off();
			}
			else{
				on();
			}
			break;
		case 'B':
			if ((PORTB&(1<<Pin)))
			{
				off();
			}
			else{
				on();
			}
			break;
		case 'C':
			if ((PORTC&(1<<Pin)))
			{
				off();
			}
			else{
				on();
			}
			break;
		case 'D':
			if ((PORTD&(1<<Pin)))
			{
				off();
			}
			else{
				on();
			}
			break;
		default:
			break;
	}
}