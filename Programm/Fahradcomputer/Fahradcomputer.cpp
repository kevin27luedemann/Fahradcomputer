/*
 * Fahradcomputer.cpp
 *
 * Created: 24.08.2015 20:25:39
 *  Author: kevin
 */ 
#define SPANNUNGSTEILER 1.985
#define VERSIONSNUMMER 1.2

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "Kompass.h"
Kompass kompass;

#include "RTC.h"
RTC rtc;
RTC zaehler;
RTC stoppuhr;

#include "Display.h"
Display oled;

#include "Interface.h"
Interface Tastatur;

#include "ADC.h"

//Anzeigebits
#define Uhrflag 0
#define Timerflag 6
#define Stoppuhrflag 7

#define Fahradflag 1
#define Kompasflag 2
#define kompaskalibrierenflag 4
#define Kompasgaineinstellenflag 5

#define Einstellungsflag 3
#define menueflag 13
#define blinkflag 14
#define refreshdisplay 15

uint16_t anzeige;	//Flagregister fuer die Anziegenschaltung
uint8_t pos;		//Handler fuer die Einganbe von Zahlen
//Ausgelagerte Sammlung der einzelnen Seitenlayouts
#include "Seiten.h"

ISR(TIMER2_OVF_vect){	//Vektor fuer die RTC
	TCNT2=TIMER2RTCTIME;
	rtc.HundSekunden++;
	rtc.interupts|= (1<<hundinterupt);
}

void initialisierung();

void maininterupthandler();

void anzeigehandler();

void eingabehandler(uint8_t taste);

int main(void)
{
	//initialisierung
	initialisierung();
	
    while(1)
    {
		//Steuerung der interupts durch Handler
		maininterupthandler();
		//Steuerung der Anzeige alle Sekunde zur aktualisierung maximale Aktulisierungsrate ist 1Hz
		anzeigehandler();
		//Tasthandler
		eingabehandler(Tastatur.Taster());
    }
}

void initialisierung(){
	//nullen der Flagregister
	rtc.interupts=0;
	anzeige=0;
	pos=0;
	//Ausgaenge und Eingaenge einstellen
	DDRD = (1<<PIND0) | (1<<PIND1) | (1<<PIND2) | (1<<PIND3);	//Pins zur Ausgabe
	DDRD &= ~((1<<PIND4) | (1<<PIND5) | (1<<PIND6));			//Restliche Pins als Eingaenge schalten
	//I2C Interface
	//twi_init();
	//Display
	oled.InitializeDisplay();
	//initialisierung wird im konstruktor vorgenommen
	oled.clearFrame();
	//HMC5883L initialisieren
	//kompass.initialize_HMC5883L();
	//initialisierung wird im Konstruktor erledigt
	
	//ADC Initialisieren auf CH0
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC) ) {
	}
	(void) ADCW;
	//Wilkommensanzeige
	willkommenpage();
	//Uhreinstellungen
	//uhreinstellen();
	//*************************************************
	//Nur Test, damit es schneller geht beim Start
	rtc.dummyeinst();
	//*********************************
	rtc.RTCstart();
	//ausgabe starten
	anzeige|=(1<<Uhrflag);
	sei();
}

void maininterupthandler(){
	//interupt dierekt aus der rtc
	if((rtc.interupts & (1<<hundinterupt))){
		rtc.zeit();
		rtc.interupts&=~(1<<hundinterupt);
	}
}

void anzeigehandler(){
	//Handler fuer 1Hz Flag
	if ((rtc.interupts & (1<<sekundeninterupt)))
	{
		//Handler fuer die Anzeige der Seiten
		//Die einzelnen Funktionen beschreiben nur den Framebuffer und nicht dierekt das Display
		if ((anzeige&(1<<menueflag)) && (anzeige&(1<<Uhrflag)))
		{
			menue_uhr();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<menueflag)) && (anzeige&(1<<Einstellungsflag)))
		{
			menue_einst();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<menueflag)))
		{
			menue_haupt();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Kompasgaineinstellenflag)))
		{
			Gaineinstellen();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Uhrflag)))
		{
			uhranzeigen();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Kompasflag)))
		{
			anzeige_kompass(kompass.angle());
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Fahradflag)))
		{
			fahradschirm(12.3,kompass.angle());
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Einstellungsflag)) && (anzeige&(1<<Timerflag)))
		{
			timerseite();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Stoppuhrflag)))
		{
			if ((anzeige&(1<<Einstellungsflag)))
			{
				stoppuhr.Sekunden++;
				stoppuhr.zeit();
			}
			Stoppuhrseite();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Timerflag)))
		{
			zaehler.Sekunden--;
			if(zaehler.timer()){
				anzeige|=(1<<blinkflag);
			}
			timerseite();
			anzeige|=(1<<refreshdisplay);
		}
		rtc.interupts&=~(1<<sekundeninterupt);
	}
	//neuen Framebuffer an das Dispay senden
	if ((anzeige&(1<<refreshdisplay)))
	{
		oled.sendFrame();
		if ((anzeige&(1<<blinkflag)))
		{
			oled.invert(rtc.Sekunden%2);
		}
		anzeige&=~(1<<refreshdisplay);
	}
}

void eingabehandler(uint8_t taste){
	if (taste!='A')
	{
		if (anzeige==255)
		{
			oled.ssd1306_command(SSD1306_DISPLAYON);
			//entweder display aus oder das Menue starten
			anzeige=(1<<menueflag);
		}
		//gleicher Handler wie oben, aber hier speziell fuer die Bearbeitung von Tastereingaben mit switch fuer die Tasten
		//es sind einfach so viele Tasten moeglich 
		else if ((anzeige&(1<<menueflag)) && (anzeige&(1<<Uhrflag)))	//Menue der Uhr
		{	
			anzeige&=~((1<<menueflag)|(1<<Uhrflag));
			switch (taste)
			{
				case '1':
					//Wechseln auf die Uhr
					anzeige|=(1<<Uhrflag);
					break;
				
				case '2':
					//starten der Stoppuhr APP
					stoppuhr.Sekunden=0;
					stoppuhr.Minuten=0;
					stoppuhr.Stunden=0;
					anzeige|=(1<<refreshdisplay) | (1<<Stoppuhrflag);
					break;
				
				case '3':
					//Starten der Timer APP
					zaehler.Sekunden=0;
					zaehler.Minuten=0;
					zaehler.Stunden=0;
					
					anzeige|=(1<<Timerflag) | (1<<Einstellungsflag);
					break;
				
				case '4':
					//Starten der Alarmapp, momentan nicht implementiert
					oled.clearFrame();
					anzeige|=(1<<refreshdisplay);
					break;
				
				default:
					//menueflag erneu setzen
					anzeige|=(1<<menueflag)|(1<<Uhrflag);
					break;
			}
		}
		else if ((anzeige&(1<<menueflag)) && (anzeige&(1<<Einstellungsflag)))	//Menue der Einstellungen
		{
			anzeige&=~((1<<menueflag)|(1<<Einstellungsflag));
			switch (taste)
			{
				case '1':
					//zuruecksetzen der Kalibrierung, da passive Kallibrierung eingebaut
					kompass.kallibrierung_ruecksetzen();
					anzeige|=(1<<Kompasflag);
					break;
				case '2':
					//Gaineinstellung fuer den Kompass
					anzeige|=(1<<Kompasgaineinstellenflag);
					break;
				case '3':
					//dies ist eine der wenigen Funktinen, die die Handler Strucktur nicht anwenden, da sie die rtc anhaellt
					uhreinstellen();
					anzeige|=(1<<Uhrflag);
					break;
				default:
					//Menueflag neu setzten
					anzeige|=(1<<menueflag)|(1<<Einstellungsflag);
					break;
			}
		}
		else if ((anzeige&(1<<menueflag)))	//nur das grosse menue
		{
			anzeige&=~(1<<menueflag);
			switch (taste)
			{
				case '1':
					anzeige|=(1<<Fahradflag);
					break;
				
				case '2':
					//Taschenrechner starten
					//Monentan deaktiv, da Umstellung der Handler 
					//operation(Flieskommazahleingabe());
					oled.clearFrame();
					anzeige|=(1<<refreshdisplay);	
					break;
				
				case '3':
					anzeige|=(1<<Kompasflag);
					break;
				
				case '4':
					//Menue der Uhr
					anzeige|=(1<<menueflag) | (1<<Uhrflag);
					break;
				
				case '5':
					//menue der Einstellungen
					anzeige|=(1<<menueflag) | (1<<Einstellungsflag);
					break;
				
				case '6':
					//Display ausschalten
					//anzeige=255;
					oled.clearFrame();
					anzeige|=(1<<refreshdisplay);
					break;
				
				default:
					//menueflag erneut setzen um abschalten zu verhindern
					anzeige|=(1<<menueflag);
					break;
			}
		}
		else if ((anzeige&(1<<Kompasgaineinstellenflag)))
		{
			anzeige&=~(1<<Kompasgaineinstellenflag);
			anzeige|=(1<<Kompasflag);
			switch (taste) {
				case '1':
					kompass.HMC5883L_command(0x01,HMCGAIN1370);
					break;
				case '2':
					kompass.HMC5883L_command(0x01,HMCGAIN1090);
					break;
				case '3':
					kompass.HMC5883L_command(0x01,HMCGAIN820);
					break;
				case '4':
					kompass.HMC5883L_command(0x01,HMCGAIN660);
					break;
				case '5':
					kompass.HMC5883L_command(0x01,HMCGAIN440);
					break;
				case '6':
					kompass.HMC5883L_command(0x01,HMCGAIN390);
					break;
				case '7':
					kompass.HMC5883L_command(0x01,HMCGAIN330);
					break;
				case '8':
					kompass.HMC5883L_command(0x01,HMCGAIN230);
					break;
				default:
					anzeige &=~(1<<Kompasflag);
					anzeige|=(1<<Kompasgaineinstellenflag);
					break;
			}
		}
		else if ((anzeige&(1<<Einstellungsflag)) && (anzeige&(1<<Timerflag)))
		{
			switch (pos)
			{
			case 0:
				switch (taste)
				{
					case '#':
						break;
					case '*':
						break;
					default:
						zaehler.Sekunden+=(taste-'0')*10;
						pos++;
						break;
				}
				break;
			case 1:
				switch (taste)
				{
					case '#':
					break;
					case '*':
					break;
					default:
					zaehler.Sekunden+=(taste-'0');
					pos++;
					break;
				}
				break;
			case 2:
				switch (taste)
				{
					case '#':
					break;
					case '*':
					break;
					default:
					zaehler.Minuten+=(taste-'0')*10;
					pos++;
					break;
				}
				break;
			case 3:
				switch (taste)
				{
					case '#':
					break;
					case '*':
					break;
					default:
					zaehler.Minuten+=(taste-'0');
					pos++;
					break;
				}
				break;
			case 4:
				switch (taste)
				{
					case '#':
					break;
					case '*':
					break;
					default:
					zaehler.Stunden+=(taste-'0')*10;
					pos++;
					break;
				}
				break;
			case 5:
				switch (taste)
				{
					case '#':
					break;
					case '*':
					break;
					default:
					zaehler.Stunden+=(taste-'0');
					pos++;
					break;
				}
				pos=0;
				anzeige&=~(1<<Einstellungsflag);
				break;
			}
		}
		else if ((anzeige&(1<<Stoppuhrflag)))
		{
			if (taste=='*')
			{
				if ((anzeige&(1<<Einstellungsflag)))
				{
					anzeige&=~(1<<Einstellungsflag);
				}
				else{
					anzeige|=(1<<Einstellungsflag);
				}
			}
			else if (taste=='0')
			{
				stoppuhr.Sekunden=0;
				stoppuhr.Minuten=0;
				stoppuhr.Stunden=0;
			}
			else if (taste=='#')
			{
				anzeige&=~((1<<Stoppuhrflag)|(1<<Einstellungsflag));
				anzeige|=(1<<menueflag);
			}
		}
		else if ((anzeige&(1<<Uhrflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Uhrflag);
				anzeige|=(1<<menueflag);
			}
		}
		else if ((anzeige&(1<<Kompasflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Kompasflag);
				anzeige|=(1<<menueflag);
			}
		}
		else if ((anzeige&(1<<Fahradflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Fahradflag);
				anzeige|=(1<<menueflag);	
			}
		}
		else if ((anzeige&(1<<Timerflag)))
		{
			if (taste=='#')
			{
				anzeige&=~((1<<Timerflag)|(1<<blinkflag));
				oled.invert(0);	//fuer den Fall, dass es invertiert blieb (50% der Faelle)
				anzeige|=(1<<menueflag);
			}
		}
		else if (anzeige==0)
		{
			anzeige|=(1<<menueflag);
		}
		if ((anzeige&(1<<blinkflag)))
		{
			if (taste=='*')
			{
				anzeige &=~(1<<blinkflag);
				oled.invert(0);	//fuer den Fall, dass es invertiert blieb (50% der Faelle)
			}
		}
	}
}