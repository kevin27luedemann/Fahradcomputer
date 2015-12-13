/*
 * Flags.h
 *
 * Created: 28.11.2015 18:24:06
 *  Author: Lüdemann
 */ 


#ifndef FLAGS_H_
#define FLAGS_H_

//Anzeigebits
#define refreshdisplay 0

uint8_t anzeige;	//Flagregister fuer die Anziegenschaltung

#define updaterate 0

uint8_t statusreg;
uint8_t pos;		//Handler fuer die Einganbe von Zahlen

#endif /* FLAGS_H_ */