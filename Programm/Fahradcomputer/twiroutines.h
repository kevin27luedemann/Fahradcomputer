/*
 * IncFile1.h
 *
 * Created: 24.08.2015 21:37:20
 *  Author: kevin
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

void twi_init(){
	TWBR = 10; //Bit fuer 100khz an Tackrate, reicht gerade noch so
}

void twi_start(){
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
}

void twi_stop(){
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	//while(!(TWCR & (1<<TWINT)));
}
void twi_ack(){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
}
void twi_nack(){
	TWCR = (1<<TWINT) | (1<<TWEN);
	TWCR &= ~(1<<TWEA);
}

void twi_write(uint8_t data){
	while(!(TWCR & (1<<TWINT)));
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

uint8_t twi_read(uint8_t ack){
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

#endif /* INCFILE1_H_ */