/*
 * SD_test.cpp
 *
 * Created: 26.01.2016 19:49:46
 * Author : Lüdemann
 */ 

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

