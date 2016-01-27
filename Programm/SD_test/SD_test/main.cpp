/*
 * SD_test.cpp
 *
 * Created: 26.01.2016 19:49:46
 * Author : Lüdemann
 */ 

#define F_CPU		8000000
#define timerhertz	100
#define CTCVALUE	F_CPU/(timerhertz*2*1024)-1

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

extern "C" {
#include "diskio.h"	
#include "ffconf.h"
#include "ff.h"
};

volatile uint8_t Timerstat;
ISR(TIMER0_COMPA_vect){
	Timerstat++;
	disk_timerproc();	//Timer der SD Karte
}


int main(void)
{
	Timerstat = 0;
	//init Timer
	TCCR0A	|= (1<<WGM01);		//Timer im ctc Mide
	OCR0A	 = CTCVALUE;		//ctc counter ende
	TIMSK0	|= (1<<OCIE0A);
	TCCR0B	|= (1<<CS02) | (1<<CS00);	//presc=1024


	static FATFS FATFS_Obj;
	
	disk_initialize(0);
	f_mount(&FATFS_Obj,"/",0);
	
	FIL fil_obj;
	
	f_open(&fil_obj, "foo.txt", FA_WRITE);
	
	f_printf(&fil_obj, "test text %u", 1);
	
	f_close(&fil_obj);
    
	f_mount(0,"",0);
	
	
    while (1) 
    {
    }
}

