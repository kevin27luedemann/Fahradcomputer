/*
 * FWCP_MK3.cpp
 *
 * Created: 28.11.2015 18:12:54
 * Author : Lüdemann
 */ 
#define VERSIONSNUMMER 3.0
#define SPANNUNGSTEILER 2.0069
#define F_CPU 8000000
#define BATMIN 3.6

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "RTC.h"
RTC rtc;

#include "Display.h"
Display oled;

#include "Interface.h"
Interface Tastatur;

#include "SOUND.h"
SOUND Lautsprecher;

#include "LSM303D.h"
LSM303D Accelerometer;

#include "Kompass.h"
Kompass kompass;

#include "ADC.h"

#include "Output.h"
Output LED('B',PORTB1);
Output Vibrationsmotor('B',PORTB2);
Output IRLED('D',PORTD7);
Output Sound('B',PORTB3);

#include "Flags.h"

//Einbau der Festspeicherung von Einstellungswerten im EEPROM
#include "EEPROM.h"
//Ausgelagerte Sammlung der einzelnen Seitenlayouts
uint8_t FPS;
//#include "Seiten.h"
//hier wird der neue Displayhandler verwendet
#include "Monitor.h"

ISR(TIMER2_OVF_vect){	//Vektor fuer die RTC
	//TCNT2=TIMER2RTCTIME;
	rtc.Sekunden++;
	rtc.interupts|= (1<<sekundeninterupt);
}

ISR(TIMER1_COMPA_vect){
	statusreg |= (1<<updaterate);
}

#define zeitproachtzaehlungen 0.001024
#define zaehlungenprozeiteinheit 8.0
#define REEDMS 5

double geschw;
double strecke;
double maxgeschw;
uint32_t Fahrtzeit;
void geschwindigkeit(float durch){
	//Einholen der Daten und ruecksetzen
	uint16_t zaehlungen = TCNT1;
	TCNT1 = 0;
	//Berechnung
	double temp = geschw;
	double umlaufzeit = (REEDMS/1000.0+(zaehlungen/zaehlungenprozeiteinheit)*zeitproachtzaehlungen);
	geschw = (durch)*M_PI*3.6;
	geschw /= umlaufzeit;
	//Mittelwert aus der letzten Messung zum Fehler minimieren
	geschw += temp;
	geschw /= 2.0;
	if (geschw >=160)
	{
		geschw=0;
	}
}

uint8_t reed_debounce(volatile uint8_t *port, uint8_t pin)
{
	if ( (*port & (1 << pin)) )
	{
		/* Pin wurde auf Masse gezogen, ms warten   */
		_delay_ms(REEDMS);
		if ( !(*port & (1 << pin)) )
		{
			/* Anwender Zeit zum Loslassen des Tasters geben */
			_delay_us(1);
			return 1;
		}
	}
	return 0;
}

void initialisierung();
void inittimer(uint8_t stat);
void maininterupthandler(uhr *mon);
void anzeigehandler();
void eingabehandler(uint8_t taste);


int main(void)
{
    initialisierung();
    uhr grosseUHR(&oled,&rtc);
	while (1) 
    {
		maininterupthandler(&grosseUHR);
    }
}

void initialisierung(){
	//nullen der Flagregister
	rtc.interupts=0;
	anzeige=0;
	statusreg=0;
	pos=0;
	FPS=0;
	//initialisieren des Zaehler fuer die Winkelgeschw sowie den Timer
	geschw=0;
	strecke = 0;
	maxgeschw = 0;
	Fahrtzeit = 0;
	inittimer(1);
	//Tastatur inm Contruktor initialisiert
	//Eingang fuer den Reedkontak schalten mit internem Pullup
	DDRA |= ((1<<PORTA6));
	PORTA |= (1<<PORTA6);
	//Display
	oled.InitializeDisplay();
	//initialisierung wird im konstruktor vorgenommen
	oled.clearFrame();
	
	//ADC Initialisieren auf CH0
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC) ) {}
	(void) ADCW;
	//Wilkommensanzeige
	//willkommenpage();
	//Uhreinstellungen
	//uhreinstellen();
	//*************************************************
	//Nur Test, damit es schneller geht beim Start
	rtc.Sekunden= 0;
	rtc.Minuten	= EEPROM_Read(EEMINUTEN);
	rtc.Stunden	= EEPROM_Read(EESTUNDEN);
	rtc.Tag		= EEPROM_Read(EETAGE);
	rtc.Monat	= EEPROM_Read(EEMONAT);
	rtc.Jahr	= EEPROM_Read(EEJAHR);
	rtc.ausgabedatumneu();
	//rtc.dummyeinst();
	//*********************************
	rtc.RTCstart();
	//ausgabe starten
	rtc.interupts|=(0<<minuteninterupt)|(0<<sekundeninterupt);
	inittimer(2);
	anzeige|=(1<<Uhrflag);
	sei();
}

void inittimer(uint8_t stat){
	TCNT1 = 0;
	if (stat==1)
	{
		TIMSK1 = 0;
		TCCR1B = ((1<<CS12) | (1<<CS10));
	}
	else if (stat==2)
	{
		OCR1A = 2603*2;
		TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);
		TIMSK1 = (1<<OCIE1A);
	}
}

void maininterupthandler(uhr *mon){
	if ((rtc.interupts&(1<<sekundeninterupt)))
	{
		rtc.zeit();
		mon->draw();
	}
	if (rtc.interupts&(1<<minuteninterupt))
	{
		//Anzeige der Uhr Programmieren
		mon->draw();
	}
	/*
	if ((anzeige&(1<<Fahradflag)))
	{
		//debounce Funktion fuer den Reedswitch
		if (reed_debounce(&PINA,PINA7))
		{
			//Durchmesser ist 28 Zoll
			geschwindigkeit(28.0*2.54/100.0);
		}
		//Hier mit werden geschwindigkeiten, die kleiner als 2.6km/h betragen gefiltert
		//Somit wird die letzte Geschwindigkeit nach 3 Sekunden geloescht
		//Das ist noch nicht die beste Variante
		else if (TCNT1>23437)
		{
			TCNT1=0;
			geschw=0;
		}
	}
	if ((rtc.interupts&(1<<Weckeractiv)))
	{
		if ((rtc.interupts&(1<<Weckerein)))
		{
			anzeige |= (1<<blinkflag) | (1<<refreshdisplay);
			rtc.interupts &= ~(1<<Weckerein);
		}
	}
	if ((anzeige&(1<<refreshdisplay)))
	{
		oled.sendFrame();
		anzeige&=~(1<<refreshdisplay);
	}*/
}

