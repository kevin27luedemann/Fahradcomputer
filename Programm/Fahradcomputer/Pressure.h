/* 
* Pressure.h
*
* Created: 08.09.2015 16:10:47
* Author: kevin
*/

//Using a LPS25H Sensor
#ifndef __PRESSURE_H__
#define __PRESSURE_H__

//Adresses of the Device
#define LPS25H_SA0_READ		0xB9
#define LPS25H_SA0_Write	0xB8
#define LPS25H_SA1_READ		0xBB
#define LPS25H_SA1_Write	0xBA

//For use of autoincrement set 8th bit to one while write register adresse
#define autoincrement 7

//Register adresses and Bits
//Refferenzregister for Pressure, default = 0, 24bit
#define REF_P_XL	0x08
#define REF_P_L		0x09
#define REF_P_H		0x0A
//Resolution for Temperature an Pressure register, select Number of averages
#define RES_CONF	0x10
#define AVGP0	0
#define AVGP1	1
#define AVGT0	2
#define AVGT1	3
/*
	AVGT1	AVGT0	| Number of			| Number of 
	AVGP1	AVGP0	| Average Pressure	| Average Temperature
----------------------------------------------------------------
	0		0		|	8				|	8
	0		1		|	32				|	16
	1		0		|	128				|	32
	1		1		|	512				|	64
*/
//Control Register
#define CTRL_REG1	0x20
#define PD	7		//Power down mode, 1= device active
#define DIFF_EN	3	//Interrupt circuit enable
#define BDU	2		//Block data Update, 1 for lock while reading
#define RESET_AZ 1	//Reset AutoZero funktion, 1 reset
#define SIM	0		//SPI interface, (0=4 wire, 1=3 wire)
//Outputrate selection
#define ODR2	6
#define ODR1	5
#define ODR0	4
/*
	ODR2	ODR1	ODR0	| Hz
---------------------------------------
	0		0		0		| one Shot
	0		0		1		| 1
	0		1		0		| 7
	0		1		1		| 12.5
	1		0		0		| 25
	1		0		1		| Reserved
	1		1		0		| Reserved
	1		1		1		| Reserved
*/
#define CTRL_REG2	0x21
#define BOOT	7	//Reboot Memory, default 0
#define FIFO_EN	6	//FIFO Enable, default 0
#define WTM_EN	5	//Enable FIFO Watermark level, default 0
#define FIFO_MEAN_DEC	4	//Enable 1Hz Odr decimation
#define SWRESET	2	//Sotware Reset, default 0
#define AUTO_ZERO	1	//Autozero enable, default 0
#define ONE_SHOT	0	//One shot enable,default 0
#define CTRL_REG3	0x22
#define INT_H_L	7	//Interrupt active high low, default 0 (active high)
#define PP_OD	6	//PUSH-PULL/open drain on interrupt pad, default 0
//Interrupt selection for INT1 pin
#define INT1_S2	1
#define INT1_S1	0
/*
	INT1_S2	INT1_S1 | INT1_Pin
------------------------------------------
	0		0		| Data signal (see CTRL_REG4)
	0		1		| Pressure high (P_high)
	1		0		| Pressure low (P_low)
	1		1		| Pressure low OR high
*/
#define CTRL_REG4	0x23	//signal on INT1 pin
#define P1_EMPTY	3	//Emptysignal
#define P1_WTM	2	//Watermark signal
#define P1_overrun	1	//Overrun signal
#define P1_DRDY	0	//Data ready signal
#define INTERRUPT_CFG	0x24
#define LIR	2	//Latch Interrupt request into INT_Source, default 0
#define PL_E	1	//Enable Interrupt event on pressure low, default 0
#define PH_E	0	//Enable Interrupt event on pressure high, default 0
#define INT_SOURCE	0x25
#define IA	2	//Interupt Active, default 0 no interrupt generated
#define PL	1	//Differential Pressure low Interrupt generated, default 0
#define PH	0	//Differential Pressure high Interrupt generated, default 0
#define STATUS_REG	0x27
#define T_DA	0	//Temperature data available, default 0
#define P_DA	1	//Pressure data available, default 0
#define T_OR	4	//Temperature data overrun, default 0
#define P_OR	5	//Pressure data overrun, default 0
//output of Pressure, 24bit
#define PRESS_OUT_XL	0x28
#define PRESS_OUT_L	0x29
#define PRESS_OUT_H	0x2A
//output of Temperature, 16bit
#define TEMP_OUT_L	0x2B
#define TEMP_OUT_H	0x2C
#define	FIFO_CTRL	0x2E
//FIFO Mode selection, see Table
#define F_MODE2	7
#define F_MODE1	6
#define F_MODE0	5
/*
	F_MODE2	F_MODE1	F_MODE0	| Mode
--------------------------------------------------------------
	0		0		0		| BYPASS Mode
	0		0		1		| FIFO Mode, stop when full
	0		1		0		| STREAM Mode, keep newest in FIFO
	0		1		1		| STREAM Mode until trigger deasserted, then change to FIFO Mode
	1		0		0		| BYPASS Mode until trigger deaserted, then change to FIFO Mode
	1		0		1		| Reserved
	1		1		0		| FIFO_MEAN Mode: FIFO is used to generate a running average filtered pressure
	1		1		1		| BYPAS mode until trigger deaserted then change to FIFO Mode
*/
//FIFO Threshhold, Watermark level setting, see Table
#define WTM_POINT4	4
#define WTM_POINT3	3
#define WTM_POINT2	2
#define WTM_POINT1	1
#define WTM_POINT0	0
/*
	WTM_POINT4..0	| FIFO_MEAN_MODE Sample Size
-------------------------------------------------------
	00001			| 2 samples moving avarage
	00011			| 4 samples
	00111			| 8 samples
	01111			| 16 samples
	11111			| 32 samples
*/
#define FIFO_STATUS	0x2F
#define WTM_FIFO 7	//Watermark status, default 0 FIFO level lover then watermark level
#define FULL_FIFO	6	//FIFO overrun bit, default 0
#define EMPTY_FIFO	5	//FIFO empty, 0 FIFO not empty
//FIFO stored data level
#define DIFF_POINT4	4
#define DIFF_POINT3	3
#define DIFF_POINT2	2
#define DIFF_POINT1	1
#define DIFF_POINT0	0
//Pressure Threshhold, 16bit!!!
#define THS_P_L		0x30
#define THS_P_H		0x31
//Pressure offset (after Soldering) 16bit!!!
#define RPDS_L		0x39
#define RPDS_H		0x3A

/*
FIFO Mode description:
	BYPASS Mode	[000]: No FIFO
	FIFO Mode	[001]: FIFO in use, stops measuring when full
	BYPASS TO STREAM Mode	[100]: FIFO in Bypass untill trigger event, then Stream Mode
	FIFO MEAN Mode	[110]: FIFO in Stream Mode an moving average in PRESS_OUT Register for noise reduction
	BYPASS TO FIFO	[111]: FIFO switch from BYPASS to FIFO mode, when the event asserted
When FIFO is active auto increment roll back to 0x28 after reading 0x2A for quick reading of entire FIFO
*/

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "I2C.h"

class Pressure
{
//variables
public:
	long int Press;
	float Tempera;
protected:
private:
	int8_t Tempoffset;
	I2C i2c;

//functions
public:
	Pressure();
	~Pressure();
	void LPS25H_initialize();
	//no other Modes set yet, later
	void READ_Pressure_once();
	void READ_Temperature();
	//For setting Mode it is important to read the register and then set the Mode
	//reducing errors while trying to change something
protected:
private:
	Pressure( const Pressure &c );
	Pressure& operator=( const Pressure &c );
	void LPS25H_command(uint8_t add, uint8_t command);

}; //Pressure

#endif //__PRESSURE_H__
