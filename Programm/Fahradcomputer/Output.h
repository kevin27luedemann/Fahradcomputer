/* 
* Output.h
*
* Created: 09.09.2015 16:01:30
* Author: kevin
*/


#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <avr/io.h>
#include <stdio.h>

class Output
{
//variables
public:
protected:
private:
	uint8_t Port;
	uint8_t Pin;

//functions
public:
	Output(uint8_t Portadresse, uint8_t Pinnumber);
	~Output();
	void on();
	void off();
	void toggle();
protected:
private:
	Output( const Output &c );
	Output& operator=( const Output &c );

}; //Output

#endif //__OUTPUT_H__
