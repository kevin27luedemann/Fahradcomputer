/* 
* LSM303D.cpp
*
* Created: 09.09.2015 08:54:18
* Author: kevin
*/


#include "LSM303D.h"

// default constructor
LSM303D::LSM303D()
{
	//Variablen initialisieren
	Tempera=0;
	roll=0.0;
	pitch=0.0;
	for (uint8_t i=0;i<3;i++)
	{
		achsen_A[i]=0;
	}
	
	//ACC init
	acce_init();
} //LSM303D

// default destructor
LSM303D::~LSM303D()
{
} //~LSM303D

void LSM303D::acce_init(){
	//ACC Activ und 25Hz ohne Block dataread
	LSM303_command(CTRL1,(1<<AXEN)|(1<<AYEN)|(1<<AZEN)|(1<<AODR2));
	
	//ACC fuer +-2g
	LSM303_command(CTRL2,0x00);
	
	//Temperatursensor an
	LSM303_command(CTRL5,(1<<TEMP_EN));
	
	//FIFO Bypass Mode
	ACCBypassmode();
}

void LSM303D::ACCStreammode(){
	LSM303_command(FIFO_CTRL,(1<<FM1));
}

void LSM303D::ACCBypassmode(){
	LSM303_command(FIFO_CTRL,0x00);
}

void LSM303D::readacc(){
	cli();
	uint8_t input[6];
	i2c.twi_start();
	i2c.twi_write(LSM303D_SA1_Write);
	i2c.twi_write(OUT_X_L_A|(1<<7));
	i2c.twi_start();
	i2c.twi_write(LSM303D_SA1_READ);
	
	for(uint8_t i=0; i<6;i++){
		if (i<5)
		{
			input[i]=i2c.twi_read(1);
		}
		else
		{
			input[i]=i2c.twi_read(0);
		}
		
	}
	i2c.twi_stop();
	achsen_A[2] =(int16_t) (input[1]<<8 | input[0]);
	achsen_A[0] =(int16_t) (input[3]<<8 | input[2]);
	achsen_A[1] =(int16_t) (input[5]<<8 | input[4]);
	pitch=atan2(achsen_A[2],achsen_A[1])-M_PI_2;
	roll=atan2(achsen_A[2],achsen_A[0])-M_PI_2;
	sei();
}

void LSM303D::readtempera(){
	cli();
	uint8_t Wert[2];
	i2c.twi_start();
	i2c.twi_write(LSM303D_SA1_Write);
	i2c.twi_write(TEMP_OUT_L_A|(1<<7));
	i2c.twi_start();
	i2c.twi_write(LSM303D_SA1_READ);
	for (uint8_t i=0;i<2;i++)
	{
		if (i<1)
		{
			Wert[i] = i2c.twi_read(1);
		}
		else{
			Wert[i] = i2c.twi_read(0);
		}
	}
	i2c.twi_stop();
	Tempera = (int16_t) (Wert[0]|(Wert[1]<<8));
	Tempera *= -1;
	sei();
}

void LSM303D::LSM303_command(uint8_t reg, uint8_t c){
	i2c.twi_start();
	i2c.twi_write(LSM303D_SA1_Write);
	i2c.twi_write(reg);
	i2c.twi_write(c);
	i2c.twi_stop();
}
