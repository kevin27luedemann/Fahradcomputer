/* 
* I2C.cpp
*
* Created: 02.09.2015 12:06:52
* Author: kevin
*/


#include "I2C.h"

// default constructor
I2C::I2C()
{
	//TWBR = 10; //Bit fuer 100khz an Tackrate, reicht gerade noch so
	TWBR=2; //fuer 8MHz und 400kHz in SCL
} //I2C

// default destructor
I2C::~I2C()
{
} //~I2C

void I2C::twi_start(){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
}
void I2C::twi_stop(){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	//while(!(TWCR & (1<<TWINT)));
}
void I2C::twi_ack(){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
}
void I2C::twi_nack(){
	TWCR = (1<<TWINT) | (1<<TWEN);
	TWCR &= ~(1<<TWEA);
}

void I2C::twi_write(uint8_t data){
	while(!(TWCR & (1<<TWINT)));
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

uint8_t I2C::twi_read(uint8_t ack){
	if (ack)
	{
		TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	}
	else{
		TWCR = (1<<TWINT) | (1<<TWEN);
	}
	
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}