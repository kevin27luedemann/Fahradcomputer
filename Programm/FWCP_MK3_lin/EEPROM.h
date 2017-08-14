/*
 * EEPROM.h
 *
 * Created: 16.10.2015 09:05:41
 *  Author: kevin
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_
#include <avr/eeprom.h>
#define   EEPReadByte(addr)         eeprom_read_byte((uint8_t *)addr)     
#define   EEPWriteByte(addr, val)   eeprom_write_byte((uint8_t *)addr, val)
//Definition fuer die Adressen um global zu speichern
//RTC (alle 8 Bit)
#define EEMINUTEN	0
#define EESTUNDEN	1
#define EETAGE		2
#define EEMONAT		3
#define EEJAHR		4
#define EEWECKMINUTEN	5
#define EEWECKSTUNDEN	6
#define EEGPSSTART	8
#define EEGPSEND 	48
#define EEGPSHOME	8
#define EEGPSGOET	16
#define EEGPSPARK	24
#define EEGPSLAST1	32
#define EEGPSLAST2	40

void EEPROM_write_float(uint16_t adresse, float data){
	union {float f; char b[4];} fb;

	fb.f = data;
	for(uint8_t i=0; i<4; i++){EEPWriteByte(adresse+i,fb.b[i]);}
}

float EEPROM_read_float(uint16_t adresse){
	union {float f; char b[4];} fb;

	fb.f = 0.;
	for(uint8_t i=0; i<4; i++){fb.b[i]=EEPReadByte(adresse+i);}

	return fb.f;
}

#endif /* EEPROM_H_ */
