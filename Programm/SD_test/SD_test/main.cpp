/*
 * SD_test.cpp
 *
 * Created: 26.01.2016 19:49:46
 * Author : Lüdemann
 */ 

#define F_CPU		8000000
#define timerhertz	100

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

extern "C" {
	#include "ffconf.h"
	#include "diskio.h"	
	#include "ff.h"
};

#include "Display.h"
Display oled;

volatile uint8_t Timerstat;
ISR(TIMER0_COMPA_vect){
	Timerstat++;
	disk_timerproc();	//Timer der SD Karte
}


int main(void)
{
	char buffer[20];
	uint8_t buffersize;
	Timerstat = 0;
	int status;
	//init Timer
	TCCR0A	 = (1<<WGM01);		//Timer im ctc Mide
	OCR0A	 = 38;		//ctc counter ende
	TIMSK0	|= (1<<OCIE0A);
	TCCR0B	|= (1<<CS02) | (1<<CS00);	//presc=1024
	sei();
	oled.clearFrame();
	oled.sendFrame();
	
	for (uint8_t i = 0; i<20; i++)
	{
		_delay_ms(100);
	}

	oled.draw_ASCI('1',1*charsize,0*charhighte);
	oled.sendFrame();
	
	FATFS FATFS_Obj;
	oled.draw_ASCI('2',2*charsize,0*charhighte);
	oled.sendFrame();
	
	status = disk_initialize(0);
	buffersize=sprintf(buffer,"%i",status);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize+70,1*charhighte);}
	oled.draw_ASCI('3',3*charsize,0*charhighte);
	oled.sendFrame();

	/*
	status = f_mkfs("0/", 0, 0);
	oled.draw_ASCI('0',0*charsize,0*charhighte);
	buffersize=sprintf(buffer,"%i",status);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	oled.sendFrame();
	*/
	status = f_mount(&FATFS_Obj,"",0) != 0;
	buffersize=sprintf(buffer,"%i",status);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize+70,2*charhighte);}
	oled.draw_ASCI('4',4*charsize,0*charhighte);
	oled.sendFrame();
	
	/* Get volume label of the default drive */
	char str[12];
	status = f_getlabel("", str, 0);
	for(uint8_t i=0;i<12;i++){oled.draw_ASCI(str[i],i*charsize+50,4*charhighte);}
	buffersize=sprintf(buffer,"%i",status);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize+70,5*charhighte);}
	
	
	FIL fil_obj;
	oled.draw_ASCI('5',5*charsize,0*charhighte);
	oled.sendFrame();
	
	status = f_open(&fil_obj, "FOO.TXT", FA_CREATE_NEW | FA_WRITE);
	buffersize=sprintf(buffer,"%i",status);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,1*charhighte);}
	oled.draw_ASCI('6',6*charsize,0*charhighte);
	oled.sendFrame();
	
	//status = f_printf(&fil_obj, "test text %u", 1);
	UINT number = 0;
	status = f_write(&fil_obj,"Hallo",5,&number);
	oled.draw_ASCI('7',7*charsize,0*charhighte);
	buffersize=sprintf(buffer,"%i:%i",status,number);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	oled.sendFrame();
	
	status = f_close(&fil_obj);
	oled.draw_ASCI('8',8*charsize,0*charhighte);
	buffersize=sprintf(buffer,"%i:%i",status,Timerstat);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	oled.sendFrame();
    
	f_mount(0,"",0);
	oled.draw_ASCI('9',9*charsize,0*charhighte);
	oled.sendFrame();
	
	
    while (1) 
    {
    }
}

