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
	LPS25H_initialize();
} //Pressure

// default destructor
Pressure::~Pressure()
{
} //~Pressure

void Pressure::LPS25H_command(uint8_t add, uint8_t command){
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA0_Write);
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
	temp |= (1<<PD) | (1<<BDU);
	LPS25H_command(CTRL_REG1,temp);
	temp=0;
	//FIFO off, no watermark no reset, reboot memory content
	temp |= (1<<BOOT);
	LPS25H_command(CTRL_REG2,temp);
	temp=0;
	//CTRL_REG3 not set
	//CTRL_REG4 not set
	//INTERRUPT_CFG not set
	//FIFO_CTRL not set
	//no threshold used
	//no Pressure offset used
	
}

void Pressure::READ_Pressure_once(){
	i2c.twi_start();
	
	i2c.twi_write(LPS25H_SA0_Write);
	uint8_t temp =(uint8_t)(PRESS_OUT_XL | (1<<autoincrement));
	i2c.twi_write(temp);
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA0_READ);
	Press=0;
	for (uint8_t i=0;i<3;i++)
	{
		if (i<2)
		{
			Press |= (i2c.twi_read(1)<<i*8);
		}
		else{
			Press |= (i2c.twi_read(0)<<i*8);
		}
	}
	i2c.twi_stop();
}

void Pressure::READ_Temperature(){
	i2c.twi_start();
	
	i2c.twi_write(LPS25H_SA0_Write);
	i2c.twi_write((uint8_t)(TEMP_OUT_L|(1<<autoincrement)));
	i2c.twi_start();
	i2c.twi_write(LPS25H_SA0_READ);
	Tempera=0;
	for (uint8_t i=0;i<2;i++)
	{
		if (i<1)
		{
			Tempera |= (i2c.twi_read(1)<<i*8);
		}
		else{
			Tempera |= (i2c.twi_read(0)<<i*8);
		}
	}
	i2c.twi_stop();
}
