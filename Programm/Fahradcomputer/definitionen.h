/*
 * definitionen.h
 *
 * Created: 05.02.2015 07:08:04
 *  Author: Lüdemann
 */ 


#ifndef DEFINITIONEN_H_
#define DEFINITIONEN_H_

#define F_CPU 3686400

//Include
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


//Variablen
uint8_t Sekunden = 0;
uint8_t Minuten = 0;
uint8_t Stunden = 0;
uint8_t WochenTag = 0;
uint8_t WStunden[2] = {0,0};
uint8_t WMinuten[2] = {0,0};
uint8_t Licht1a[2] = {0,0};
uint8_t Licht2a[2] = {0,0};
uint8_t Licht1b[2] = {0,0};
uint8_t Licht2b[2] = {0,0};
uint8_t Wan = 0;
uint8_t sektemp=0;
float tempera;
int8_t sekoffset=-2;
int8_t tempoffset = 0;
uint8_t lampenstaerke = 3;
char Buffer[20];
uint8_t displayoff = 0;
bool displaystat = true;
uint8_t weckverzoegerung = 20;
bool Wochenendeweckeran = true;
bool Werktagsweckeran = true;


//Strings um Speicher zu sparen
#define NAME(x) #x
const char lampenstaerkestring[] = "Lampenstaerke:";
const char sekoffsetstring[] = "Sekundenoffset:";
const char tempoffsetstring[] = "Temperaturoffset:";
const char weckverzoegerungstring[] = "Weckerverzoege:";
const char Werktagsweckeranstring[] = "Wecktagswe:";
const char Wochenendeweckeranstring[] = "Wochenendswe:";
const char Stundenstring[] = "Stunden";
const char Minutenstring[] = "Minuten";
const char WochenTagstring[] = "WochenTag";
const char WStunden0string[] = "WStunden WE";
const char WMinuten0string[] = "WMinuten WE";
const char WStunden1string[] = "WStunden WO";
const char WMinuten1string[] = "WMinuten WO";
const char WeckerWEstring[] = "Werktags";
const char WeckerWOstring[] = "Wohenende";
const char Zeitstring[] = "Zeit";
const char Weckzeitstring [] = "Weckzeit: ";
const char begruesung[] = "Guten Tag";
const char versionstring[] = "Version 2.9";



//Funktionen
void zeit();
inline uint8_t debounce(volatile uint8_t *port, uint8_t pin);
void zeitsetzen();
void init();
void Wecker();
void init();
void zeitsetzen();
uint16_t ADC_Read( uint8_t channel );
uint16_t ADC_Read_Avg( uint8_t channel, uint8_t nsamples );
uint8_t einst(uint8_t posit);
void erhoehen(uint8_t *var, uint8_t obergrenze, uint8_t pos, const char *data);
void wtag(uint8_t numm);
void aus(uint8_t zeile, uint8_t pos);
void zeitaus(uint8_t pos);
uint8_t menu();
void temperatur(uint8_t);
void lichteinaus(uint8_t pos);

#endif /* DEFINITIONEN_H_ */

inline uint8_t debounce(volatile uint8_t *port, uint8_t pin)
{
	if ( !(*port & (1 << pin)) )
	{
		/* Pin wurde auf Masse gezogen, 100ms warten   */
		_delay_ms(50);   // Maximalwert des Parameters an _delay_ms
		_delay_ms(10);   // beachten, vgl. Dokumentation der avr-libc
		if ( (*port & (1 << pin)) )
		{
			/* Anwender Zeit zum Loslassen des Tasters geben */
			_delay_ms(50);
			_delay_ms(10);
			return 1;
		}
	}
	return 0;
}
