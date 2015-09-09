/* 
* LSM303D.h
*
* Created: 09.09.2015 08:54:18
* Author: kevin
*/

//Class for Accelerometer and Magnetometer on LSM303D chip
//at the Moment only Accelerometer is in use
#ifndef __LSM303D_H__
#define __LSM303D_H__

//I2C Adress
#define LSM303D_SA0_READ	0x3D
#define LSM303D_SA0_Write	0x3C
#define LSM303D_SA1_READ	0x3B
#define LSM303D_SA1_Write	0x3A

//Register and Bits (at the Moment only Accelerometer)
//Temperature output (only active after request)
#define TEMP_OUT_L	0x05
#define TEMP_OUT_H	0x06
#define STATUS_M	0x07	//Status Register for Temperature an Megnetometer (not in use)
//Outputregister for Magnetometer not in use

//Accelerometer Registers
#define REFERENCE_X	0x1C	//Reference value for high-pass Filter
#define REFERENCE_Y	0x1D
#define REFERENCE_Z	0x1E
#define CTRL0	0x1F	//Controlregister 0
#define BOOT	7	//Reboot Memory, default 0
#define FIFO_EN	6	//FIFO Enable, default 0
#define FTH_EN	5	//FIFO Threshold enable, default 0
#define HP_CLICK	2	//High-pass filter enable for Click Function, default 0
#define HPIS1	1	//High-pass filter enable for interrupt generator 1, default 0
#define HPIS2	0	//High-pass filter enable for interrupt generator 2, default 0
#define CTRL1	0x20	//Controlregister 1
#define AXEN	0	//Enable Accelerometer X-Achse, default 1
#define AYEN	1	//Enable Accelerometer Y-Achse, default 1
#define AZEN	2	//Enable Accelereomter Z-Achse, default 1
#define BDU	3	//Block data Update for Both Sensors, default 0, lock at 1
//Acceleration data Selection, default 0000 (Power down), see Table
#define	AODR0	4
#define AODR1	5
#define AODR2	6
#define AODR3	7
/*
	AODR3	AODR2	AODR1	AODR0	| Power Mode [Hz]
--------------------------------------------------------------
	0		0		0		0		|	Power down
	0		0		0		1		|	3.125
	0		0		1		0		|	6.25
	0		0		1		1		|	12.5
	0		1		0		0		|	25
	0		1		0		1		|	50
	0		1		1		0		|	100
	0		1		1		1		|	200
	1		0		0		0		|	400
	1		0		0		1		|	800
	1		0		1		0		|	1600
*/
#define CTRL2	//Controlregister 2
#define SIM	0	//SPI Mode, default 0 = 4 wire Mode
#define AST	1	//Acceleration self-test enable, default 0
//Acceleration full-scale selection, default 0
#define AFS0	3
#define AFS1	4
#define AFS2	5
/*
	AFS0	AFS1	AFS2	| full-scale [+-g]
-------------------------------------------------
	0		0		0		| 2
	0		0		1		| 4
	0		1		0		| 6
	0		1		1		| 8
	1		0		0		| 16
*/
//Accelerometer anti-alias filter bandwidth
#define ABW0	6
#define ABW1	7
/*
	ABW0	ABW1	| anti-alias Bandwidth [Hz]
------------------------------------------------
	0		0		| 773
	0		1		| 194
	1		0		| 326
	1		1		| 50
*/
#define CTRL3	0x22	//Controlregister 3
#define INT1_BOOT	7	//Boot on Int1 enable, default 0
#define INT1_CLICK	6	//Click generator interrupt on INT1, default 0
#define INT1_IG1	5	//Inertial interrupt generator 1 on INT1, default 0
#define INT1_IG2	4	//Inertial interrupt generator 2 on INT2, default 0
#define INT1_IGM	3	//Magnetic interrupt generator on INT1, default 0
#define INT1_DRDY_A	2	//Accelerometer data-ready signal in INT1, default 0
#define INT1_DRDY_M	1	//Magnetometer data-ready signal in INT1, default 0
#define INT1_EMPTY	0	//FIFO empty indication on INT1, default 0
#define CTRL4	0x23
#define INT2_CLICK	7	//Click generator interrupt on INT2, default 0
#define INT2_IG1	6	//Inertial interrupt generator 1 on INT2, default 0
#define INT2_IG2	5	//Inertial interrupt generator 2 on INT2, default 0
#define INT2_IGM	4	//Magnetic interrupt generator on INT2, default 0
#define INT2_DRDY_A	3	//Accelerometer data-ready signal on INT2, default 0
#define INT2_DRDY_M 2	//Magnetometer data-ready signal on INT2, default 0
#define INT2_OVERRUN	1	//FIFO overrun interrupt on INT2, default 0
#define INT2_FTH	0	//FIFO Threshold interupt on INT2, default 0
#define CTRL5	0x24	//Controlregister 5
#define TEMP_EN	7	//Temperature sensor enable, default 0 (disable)
#define LIR2	1	//Latch interrupt on INT2
#define LIR1	0	//Latch interrupt on INT1
//Magnetic resolution: 00 low, 11 high resolution
#define M_RES1	6
#define M_RES0	5
//Magnetic data selection, default 110
#define M_ODR2	4
#define M_ODR1	3
#define M_ODR0	2
/*
	M_ODR2	M_ODR1	M_ODR0	| Power Mode [Hz]
------------------------------------------------
	0		0		0		| 3.125
	0		0		1		| 6.25
	0		1		0		| 12.5
	0		1		1		| 25
	1		0		0		| 50
	1		0		1		| 100	//only when ODR>50Hz or Accelerometer in Power down mode
	1		1		0		| //do not use
	1		1		1		| Reserved
*/
#define CTRL6	0x25	//Controllregister 6
//Magnetic full- scale selection
#define MFS1	6
#define MFS0	5
/*
	MFS1	MFS0	| full-scale [gauss]
----------------------------------------
	0		0		| 2
	0		1		| 4
	1		0		| 8
	1		1		| 12
*/
#define CTRL7	0x26	//Controllregister 7
//High-pass filter mode selection for Accelerometer, default 00
#define AHPM1	7
#define AHPM0	6
/*
	AHPM1	AHPM0	| filter Mode
-----------------------------------------------
	0		0		| Normal Mode, (with reference)
	0		1		| Reference signal for filtering
	1		0		| Normal Mode
	1		1		| Auto-reset on interrupt event
*/
#define AFDS	5	//Filter accelerometer Data, default 0 = Bypass filter
#define T_ONLY	4	//Temperatur sensor only, default 0, at 1 Magnetometer off
#define MLP	2	//Magnetic low power Mode, default 0, at 1 ony low level Magnetometer
//Magnetic sensor Mode selection, default 10
#define MD1	1
#define MD0	0
/*
	MD1	MD0	| Mode
-----------------------------
	0	0	| Continuous-conversion
	0	1	| Single-conversion
	1	0	| Power-down
	1	1	| Power-down
*/
#define STATUS_A	0x27	//Statusregister for Accelerometer
#define ZYXAOR	7	//Accelerometer X,Y,Z data overrun, default 0
#define ZAOR	6	//Acc z data Overrun, default 0
#define YAOR	5	//ACC y data Overrun, default 0
#define XAOR	4	//ACC x data Overrun, default 0
#define ZYXADA	3	//ACC xyz new data available, default 0
#define ZADA	2	//ACC z new data, default 0
#define YADA	1	//Acc y new data, default 0
#define XADA	0	//ACC x new data, default 0
//Accelerometer Ouput register
#define OUT_X_L_A	0x28
#define OUT_X_H_A	0x29
#define OUT_Y_L_A	0x2A
#define OUT_Y_H_A	0x2B
#define OUT_Z_L_A	0x2C
#define OUT_Z_H_A	0x2D
#define FIFO_CTRL	0x2E	//FIFO COntroll register
//FIFO Threshold value, default 00000
#define FTH0	0	
#define FTH1	1
#define FTH2	2
#define FTH3	3
#define FTH4	4
//FIFO Mode selection, default 000
#define FM0	5
#define FM1	6
#define FM2	7
/*
	FM2	FM1	FM0	| FIFO Mode
-------------------------------------------
	0	0	0	| Bypass Mode
	0	0	1	| FIFO Mode
	0	1	0	| Stream Mode
	0	1	1	| Stream to FIFO Mode
	1	0	0	| Bypass to Stream Mode
*/
#define FIFO_SRC	0x2F	//FIFO Status register
#define FTH	7	//FIFO Threshold status, default 0
#define OVRN	6	//FIFO Overrun Status
#define EMPTY	5	//Empty Status
//FIFO storage level
#define FSS4	4
#define FSS3	3
#define FSS2	2
#define FSS1	1
#define FSS0	0
//CLICK-CLICK noch nicht implementier, aber spannend, mehr Forschung notwendig
//z.B. anwendung zum aufwecken nach Power-down Phase oder zum reaktivieren der main CPU


class LSM303D
{
//variables
public:
protected:
private:

//functions
public:
	LSM303D();
	~LSM303D();
protected:
private:
	LSM303D( const LSM303D &c );
	LSM303D& operator=( const LSM303D &c );

}; //LSM303D

#endif //__LSM303D_H__
