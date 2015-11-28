/*
 * EEPROM.h
 *
 * Created: 16.10.2015 09:05:41
 *  Author: kevin
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void EEPROM_Write(uint16_t adresse, uint8_t data){
	cli();	//zur sicherheit interrupts ausschalten
	while(EECR&(1<<EEPE));
	
	//schreibe adresse und data
	EEAR = adresse;
	EEDR = data;
	//fuehre schreiben aus
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
	
	sei();
}

uint8_t EEPROM_Read(uint16_t adresse){
	cli();	//zur sicherheit interrupts ausschalten
	while(EECR&(1<<EEPE));
	
	//schreibe adresse
	EEAR = adresse;
	//fuehre lesen aus
	EECR |= (1<<EERE);
	
	sei();
	return EEDR;
}

//Definition fuer die Adressen um global zu speichern
//RTC (alle 8 Bit)
#define EEMINUTEN	0
#define EESTUNDEN	1
#define EETAGE		2
#define EEMONAT		3
#define EEJAHR		4



#endif /* EEPROM_H_ */