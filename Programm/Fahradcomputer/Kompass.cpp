/* 
* Kompass.cpp
*
* Created: 02.09.2015 11:47:38
* Author: kevin
*/


#include "Kompass.h"

// default constructor
Kompass::Kompass()
{
	//Config Register A
	HMC5883L_command(0x00, HMCSAMPLES8+HMCDATARATE15+HMCMEASUREMODENORMAL);

	//Config Gain
	HMC5883L_command(0x01, HMCGAIN1090);

	//Set Mode auf single
	HMC5883L_command(0x02, HMCSIGLEMODE);
	//Variablen
	for(uint8_t i=0;i<3;i++){
		achsen[i]=0;
		offset[i]=0;
	}
} //Kompass

// default destructor
Kompass::~Kompass()
{
} //~Kompass

void Kompass::HMC5883L_command(uint8_t adresse, uint8_t command){
	i2c.twi_start();
	i2c.twi_write(HMCWRITEADRESSE);
	i2c.twi_write(adresse);
	i2c.twi_write(command);
	i2c.twi_stop();
}

void Kompass::initialize_HMC5883L(){
	//Config Register A
	HMC5883L_command(0x00, HMCSAMPLES8+HMCDATARATE15+HMCMEASUREMODENORMAL);

	//Config Gain
	HMC5883L_command(0x01, HMCGAIN1090);

	//Set Mode auf single
	HMC5883L_command(0x02, HMCSIGLEMODE);
	for (uint8_t i=0;i<3;i++)
	{
		achsen[i]=0;
		offset[i]=0;
	}
}

uint8_t Kompass::HMC5883L_readHeading(){
	cli();
	uint8_t heading[6];
	i2c.twi_start();
	i2c.twi_write(HMCWRITEADRESSE);
	i2c.twi_write(0x03);
	i2c.twi_start();
	i2c.twi_write(HMCREADADRESSE);
	
	for(uint8_t i=0; i<6;i++){
		if (i<5)
		{
			heading[i]=i2c.twi_read(1);
		}
		else
		{
			heading[i]=i2c.twi_read(0);
		}
		
	}
	i2c.twi_stop();
	achsen[0] =(int16_t) (heading[1] | (heading[0] << 8))-offset[0];
	achsen[2] =(int16_t) (heading[3] | (heading[2] << 8))-offset[2];
	achsen[1] =(int16_t) (heading[5] | (heading[4] << 8))-offset[1];
	sei();
	if (achsen[0]==-4096 || achsen[1]==-4096 || achsen[2]==-4096)
	{
		return 1;
	}
	
	return 0;
}

double Kompass::angle(){
	double angle=0;
	if (HMC5883L_readHeading())
	{
		angle=-1.0;
	}
	else{
		angle=atan2f(achsen[0],achsen[2])*180.0/M_PI+180.0;
		//deklination
		angle+=2.35;
		//Normierung auf %360
		if (angle>=360)
		{
			angle-=360;
		}
		else if (angle < 0)
		{
			angle+=360;
		}
	}
	
	HMC5883L_command(0x02,HMCSIGLEMODE);
	return angle;
}