/* 
* Pressure.cpp
*
* Created: 08.09.2015 16:10:47
* Author: kevin
*/


#include "Pressure.h"

// default constructor
Pressure::Pressure()
{
	Press = 0;
	Tempera = 0;
	Tempoffset = -4;	//Aus Erfahrung
	Pressure0=1;
	LPS25H_initialize();
} //Pressure

// default destructor
Pressure::~Pressure()
{
} //~Pressure

void Pressure::LPS25H_command(uint8_t add, uint8_t command){
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA1_Write);
	i2c.twi_write(add);
	i2c.twi_write(command);
	i2c.twi_stop();
}

void Pressure::LPS25H_initialize(){
	uint8_t temp=0;
	//internal averages Pressure and Temperature (both 8)
	temp &= ~((1<<AVGT1) | (1<<AVGT0) | (1<<AVGP1) | (1<<AVGP0)); 
	LPS25H_command(RES_CONF,temp);
	temp=0;
	//Enable device and set a single shot as well as Lock while reading
	temp |= (1<<PD) | (1<<BDU) | (1<<ODR1);
	LPS25H_command(CTRL_REG1,temp);
	temp=0;
	//FIFO on, no watermark no reset, reboot memory content, FIFO MEAN Mode
	temp |= (1<<BOOT) | (1<<FIFO_EN) | (1<<FIFO_MEAN_DEC);
	LPS25H_command(CTRL_REG2,temp);
	temp=0;
	//FIFO Controll register for MEAN Mode und 8 moving averages
	temp|=(1<<F_MODE2) | (1<<F_MODE1) | (1<<WTM_POINT0) | (1<<WTM_POINT1) | (1<<WTM_POINT2);
	LPS25H_command(FIFO_CTRL,temp);
	//CTRL_REG3 not set
	//CTRL_REG4 not set
	//INTERRUPT_CFG not set
	//no threshold used
	//no Pressure offset used
	
}

void Pressure::READ_Pressure_once(){
	uint8_t Wertedruck[3];
	cli();
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA1_Write);
	i2c.twi_write(PRESS_OUT_XL|(1<<autoincrement));
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA1_READ);
	for (uint8_t i=0;i<3;i++)
	{
		if (i<2)
		{
			Wertedruck[i] = i2c.twi_read(1);
		}
		else{
			Wertedruck[i] = i2c.twi_read(0);
		}
	}
	i2c.twi_stop();
	Press = ((double)Wertedruck[2]*65536);
	Press += ((double)Wertedruck[1]*256);
	Press += (double)Wertedruck[0];
	//Press = (Wertedruck[0] + (Wertedruck[1]*256) + (Wertedruck[2]*65536));
	Press/=4096.0;
	sei();
}

void Pressure::READ_Temperature(){
	cli();
	uint8_t Wert[2];
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA1_Write);
	i2c.twi_write(TEMP_OUT_L|(1<<autoincrement));
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA1_READ);
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
	Tempera = ((int16_t)(Wert[0]+(Wert[1]<<8)))/480.0;
	Tempera+=42.5+Tempoffset;
	sei();
}

double Pressure::altitude(double Pressurevalue){
	return (Pressure0*100*log(Pressure0/Pressurevalue)/(1.2041*9.81));
}
void Pressure::set_Pressure0(double Pressurevalue){
	Pressure0=Pressurevalue;
}