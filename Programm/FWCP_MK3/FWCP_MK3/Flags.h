/*
 * Flags.h
 *
 * Created: 28.11.2015 18:24:06
 *  Author: Lüdemann
 */ 


#ifndef FLAGS_H_
#define FLAGS_H_

//Anzeigebits
#define Fahradflag 0

#define Uhrflag 1
#define Timerflag 2
#define Stoppuhrflag 3
#define Alarmflag 4
#define Uhrflaggross 5
#define Weckeranzeigeflag 6

#define Kompasflag 7
#define kompaskalibrierenflag 8
#define Kompasgaineinstellenflag 9

#define Druckflag 10

#define Wanderflag 11

#define Einstellungsflag 12
#define menueflag 13

#define blinkflag 14
#define refreshdisplay 15

uint16_t anzeige;	//Flagregister fuer die Anziegenschaltung

#define updaterate 0

uint8_t statusreg;
uint8_t pos;		//Handler fuer die Einganbe von Zahlen

#endif /* FLAGS_H_ */