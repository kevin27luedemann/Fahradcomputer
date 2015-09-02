/*
 * HMC5883L.h
 *
 * Created: 24.08.2015 21:39:23
 *  Author: kevin
 */ 


#ifndef HMC5883L_H_
#define HMC5883L_H_

//Konstanten
#define HMCWRITEADRESSE 0x3C
#define HMCREADADRESSE 0x3D
#define HMCSIGLEMODE 0b00000001
#define HMCCONTINUOUSMODE 0b00000000
#define HMCIDLEMODE 0b00000011
#define HMCGAIN1370 0b00000000
#define HMCGAIN1090 0b00100000
#define HMCGAIN820 0b01000000
#define HMCGAIN660 0b01100000
#define HMCGAIN440 0b10000000
#define HMCGAIN390 0b10100000
#define HMCGAIN330 0b11000000
#define HMCGAIN230 0b11100000
#define HMCSAMPLES1 0b00000000
#define HMCSAMPLES2 0b00100000
#define HMCSAMPLES4 0b01000000
#define HMCSAMPLES8 0b01100000
#define HMCDATARATE0075 0b00000000
#define HMCDATARATE105 0b00000100
#define HMCDATARATE3 0b00001000
#define HMCDATARATE705 0b00001100
#define HMCDATARATE15 0b00010000
#define HMCDATARATE30 0b00011000
#define HMCDATARATE75 0b00011000
#define HMCMEASUREMODENORMAL 0b00000000
#define HMCMEASUREMODEPOSITIVE 0b00000001
#define HMCMEASUREMODENEGATIVE 0b00000010

//Variablen
int16_t achsen[3];
int16_t offset[3];

void HMC5883L_command(uint8_t adresse, uint8_t command){
	twi_start();
	twi_write(HMCWRITEADRESSE);
	twi_write(adresse);
	twi_write(command);
	twi_stop();
}

void initialize_HMC5883L(){
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

uint8_t HMC5883L_readHeading(){
	cli();
	uint8_t heading[6];
	twi_start();
	twi_write(HMCWRITEADRESSE);
	twi_write(0x03);
	twi_start();
	twi_write(HMCREADADRESSE);
	
	for(uint8_t i=0; i<6;i++){
		if (i<5)
		{
			heading[i]=twi_read(1);
		} 
		else
		{
			heading[i]=twi_read(0);
		}
		
	}
	twi_stop();
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

bool HMC5883L_STATUSRDY(void){
	cli();
	uint8_t status=0;
	twi_start();
	twi_write(HMCWRITEADRESSE);
	twi_write(0x09);
	twi_start();
	twi_write(HMCREADADRESSE);
	status=twi_read(0);
	twi_stop();
	status  &= 0x01;
	sei();
	return status;
}

uint8_t HMC5883L_GetMode(void){
	cli();
	uint8_t mode=0;
	twi_start();
	twi_write(HMCWRITEADRESSE);
	twi_write(0x02);
	twi_start();
	twi_write(HMCREADADRESSE);
	mode=twi_read(0);
	twi_stop();
	mode  &= 0x02;
	sei();
	return mode;
}

double angle(){
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


#endif /* HMC5883L_H_ */