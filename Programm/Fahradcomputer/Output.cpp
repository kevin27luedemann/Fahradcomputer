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

void Output::off(){
	switch (Port)
	{
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