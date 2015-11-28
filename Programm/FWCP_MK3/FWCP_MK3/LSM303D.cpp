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
	Schrittzaehler=0;
	gravity=0;
	higher=0;
	lower=0;
	threschold=27;	//aus Erfahrung
	for (uint8_t i=0;i<3;i++)
	{
		achsen_A[i]=0;
	}
	for (uint8_t i=0;i<50;i++)
	{
		Daten[i]=0;
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

void LSM303D::readacc_fast(){
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

void LSM303D::get_gravity(){
	uint32_t temp=0;
	uint32_t temp2[4];
	cli();
	for (uint8_t i=0;i<25;i++)
	{
		readacc_fast();
		//readacc();
		temp2[0] = achsen_A[2];
		temp2[0]*= temp2[0];
		temp2[1] = achsen_A[0];
		temp2[1]*= temp2[1];
		temp2[2] = achsen_A[1];
		temp2[2]*= temp2[2];
		temp2[3] = temp2[0]+temp2[1]+temp2[2];
		temp +=sqrt(temp2[3]);
	}
	//readacc_fast();
	sei();
	temp/=25;
	gravity=(gravity+temp)/2;
}

void LSM303D::schritt(uint8_t stat){
	uint8_t counter_L=0;
	uint8_t counter_H=0;
	uint8_t zaehler=0;
	uint8_t temp=0;
	uint32_t temp2[3];
	for (uint8_t i=0;i<25;i++)
	{
		readacc_fast();
		temp2[0] = achsen_A[2];
		temp2[0]*= temp2[0];
		temp2[1] = achsen_A[0];
		temp2[1]*= temp2[1];
		temp2[2] = achsen_A[1];
		temp2[2]*= temp2[2];
		Daten[i+stat*25] = sqrt(temp2[0]+temp2[1]+temp2[2]);
	}
	if (stat)
	{
		//THreshold ausrechnen
		for (uint8_t i=0;i<50;i++)
		{
			if (Daten[i]>=gravity)
			{
				higher+=Daten[i];
				counter_H++;
			}
			else{
				lower+=Daten[i];
				counter_L++;
			}
		}
		higher/=counter_H;
		lower/=counter_L;
		for (uint8_t i=0;i<50;i++)
		{
			if (temp == 0)
			{
				if (Daten[i]+threschold<lower)
				{
					temp = 3;
				}
				else if (Daten[i]-threschold>higher)
				{
					temp = 1;
				}
				
			}
			else if (temp == 1 && Daten[i]+threschold<higher)
			{
				zaehler++;
				temp = 2;
			}
			else if (temp == 2 && Daten[i]+threschold<lower)
			{
				temp = 3;
			}
			else if (temp == 3 && Daten[i]-threschold>lower)
			{
				zaehler++;
				temp = 4;
			}
			else if (temp == 4 && Daten[i]-threschold>higher)
			{
				temp = 1;
			}
		}
		if (zaehler==1)
		{
			Schrittzaehler++;
		}
		else{
			Schrittzaehler+=zaehler/2;
		}
	}
}
