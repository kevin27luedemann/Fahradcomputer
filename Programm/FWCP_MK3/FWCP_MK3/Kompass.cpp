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
	achsen[0] =(int16_t) (heading[1] | (heading[0] << 8));
	achsen[1] =(int16_t) (heading[3] | (heading[2] << 8));
	achsen[2] =(int16_t) (heading[5] | (heading[4] << 8));
	sei();
	if (achsen[0]==-4096 || achsen[1]==-4096 || achsen[2]==-4096)
	{
		return 1;
	}
	//Kallibrierung durchfuehren, jedes mal, wenn eine abfrage stattfindet
	//somit passive kalibrierung
	else{
		for(uint8_t i=0;i<3;i++){
			if(achsen[i]>max[i]){
				max[i]=achsen[i];
				offset[i]=(max[i]+min[i])/2;
			}
			else if(achsen[i]<min[i]){
				min[i]=achsen[i];
				offset[i]=(max[i]+min[i])/2;
			}
			else {
				achsen[i]-=offset[i];
			}
		}
	}
	
	return 0;
}

double Kompass::angle(double roll, double pitch){
	double angle=0;
	int16_t temp_M[2];
	if (HMC5883L_readHeading())
	{
		angle=-1.0;
	}
	else{
		temp_M[0] = achsen[0]*cos(pitch);
		temp_M[0]+= achsen[1]*sin(roll)*cos(pitch);
		temp_M[0]+= achsen[2]*cos(roll)*sin(pitch);
		temp_M[1] = achsen[1]*cos(roll);
		temp_M[1]+= achsen[2]*sin(roll);
		//temp_M[0]=achsen[0];
		//temp_M[1]=achsen[1];
		angle=atan2(temp_M[0],temp_M[1])*180.0/M_PI+180.0;
		//Tilt kompensation
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

void Kompass::kallibrierung_ruecksetzen(){
	for (uint8_t i=0;i<3;i++)
	{
		max[i]=0;
		min[i]=0;
		offset[i]=0;
	}
}
