/*
 * SD_test.cpp
 *
 * Created: 21.01.2016 18:38:54
 * Author : Lüdemann
 */ 
//#define __AVR_ATmega1284P__	true
#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

extern "C" {
#include "diskio.h"
#include "ffconf.h"
#include "ff.h"	
};


int main(void)
{
	static FATFS FATFS_Obj;
	
	disk_initialize(0);
	f_mount(&FATFS_Obj,"",0);
	
	FIL fil_obj;
	
	f_open(&fil_obj, "foo.txt", FA_WRITE);
	
	//f_printf(&fil_obj, "bar %d", variableName);
	
	f_close(&fil_obj);
	
    while (1) 
    {
    }
}

