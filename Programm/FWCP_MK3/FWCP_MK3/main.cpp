/*
 * FWCP_MK3.cpp
 *
 * Created: 28.11.2015 18:12:54
 * Author : Lüdemann
 */ 
#define VERSIONSNUMMER 3.01
#define SPANNUNGSTEILER 2.0069
#define F_CPU 8000000
#define BATMIN 3.6
#define zeitproachtzaehlungen 0.001024
#define zaehlungenprozeiteinheit 8.0
#define REEDMS 5

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

//sofreset erlauben und ruecksetzten des Whatchdogs
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void wdt_init(void)
{
	MCUSR = 0;
	wdt_disable();
	return;
}
#define soft_reset()        \
do                          \
{                           \
	wdt_enable(WDTO_15MS);  \
	for(;;)                 \
	{                       \
	}                       \
} while(0)

//define new and delete operator
void * operator new(size_t size)
{
	return malloc(size);
}
void operator delete(void * ptr)
{
	free(ptr);
}

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

//Anzeigebits
#define refreshdisplay 0

uint8_t anzeige;	//Flagregister fuer die Anziegenschaltung

#define updaterate 0

uint8_t statusreg;
uint8_t position;		//Numer der Aktuellen Seite fuer Array und sonstieges

//Einbau der Festspeicherung von Einstellungswerten im EEPROM
#include "EEPROM.h"
//Ausgelagerte Sammlung der einzelnen Seitenlayouts
uint8_t FPS;

//schlechte variante der Uhreinstellung
void uhreinstellen();

ISR(TIMER2_OVF_vect){	//Vektor fuer die RTC
	//TCNT2=TIMER2RTCTIME;
	rtc.Sekunden++;
	rtc.interupts|= (1<<sekundeninterupt);
}

ISR(TIMER1_COMPA_vect){
	statusreg |= (1<<updaterate);
}

double geschw;
double strecke;
double maxgeschw;
uint32_t Fahrtzeit;
void nullen(){
	geschw = 0;
	strecke = 0;
	maxgeschw = 0;
	Fahrtzeit = 0;
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
//Berechnung der Geschwindigkeit
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

//hier wird der neue Displayhandler verwendet
#include "Monitor.h"
#define numberofpages 3

void initialisierung();
void maininterupthandler(monitor *mon, uint8_t taste);

int main(void)
{
    initialisierung();
    monitor* Folien[numberofpages] =
	{
		new uhr(&oled,&rtc),
		new tacho(&oled,&rtc),
		new einstellungen(&oled,&rtc)
	};


	while (1) 
    {
		maininterupthandler(Folien[position],Tastatur.unified());
    }
}

void initialisierung(){
	//nullen der Flagregister
	anzeige=0;
	statusreg=0;
	position=0;
	FPS=0;
	
	//initialisieren des Zaehler fuer die Winkelgeschw sowie den Timer
	TCNT1 = 0;
	TIMSK1 = 0;
	TCCR1B = ((1<<CS12) | (1<<CS10));
	
	//Eingang fuer den Reedkontak schalten mit internem Pullup
	DDRA |= ((1<<PORTA6));
	PORTA |= (1<<PORTA6);
	
	//ADC Initialisieren auf CH0
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC) ) {}
	(void) ADCW;
	
	//Wilkommensanzeige
	wilkommen wil(&oled,&rtc);
	wil.draw();
	for (uint8_t i=0; i<10;i++)
	{
		_delay_ms(50);
		_delay_ms(50);
	}

	//letzten Zeitpunkt holen und RTC starten
	rtc.Sekunden= 0;
	rtc.Minuten	= EEPROM_Read(EEMINUTEN);
	rtc.Stunden	= EEPROM_Read(EESTUNDEN);
	rtc.Tag		= EEPROM_Read(EETAGE);
	rtc.Monat	= EEPROM_Read(EEMONAT);
	rtc.Jahr	= EEPROM_Read(EEJAHR);
	rtc.ausgabedatumneu();
	rtc.RTCstart();
	sei();
}

void maininterupthandler(monitor *mon, uint8_t taste){
	if (taste!=' ')
	{
		switch (taste)							//Tastendruck ueberpruefen
		{
			case 'm':
				//Menue aufrufen
				//abtrakt, da nur 2 Folien
				position++;
				if (position > numberofpages-1)
				{
					position=0;
				}
				anzeige |= (1<<refreshdisplay);
				break;
			case '0':
				//Licht an/aus
				if (LED.ison())
				{
					LED.off();
				}
				else{
					LED.on();
				}
				break;
			default:
				//alle anderen Tasten werden an den jeweiligen Handler weiter gegeben
				mon->tastendruck(&taste);
				anzeige |= (1<<refreshdisplay);
				break;
		}
	}
	
	if (position==1)							//Berechnung der Geschwindigkeit fuer Tacho
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
	
	if (statusreg&(1<<updaterate))				//24 FPS fuer schnelle anzeigen
	{
		//anzeige &= (1<<refreshdisplay);
	}
	
	if ((rtc.interupts&(1<<sekundeninterupt)))	//Sekunden
	{
		rtc.zeit();
		if (position==1)
		{
			if (geschw>maxgeschw)
			{
				maxgeschw=geschw;
			}
			if (geschw >= 1.0)
			{
				Fahrtzeit++;
			}
			strecke+=geschw/3.6;
		}
		anzeige |= (1<<refreshdisplay);
		rtc.interupts &= ~(1<<sekundeninterupt);
	}
	if ((rtc.interupts&(1<<minuteninterupt)))		//Minuten
	{
		anzeige |= (1<<refreshdisplay);
		rtc.interupts &= ~(1<<minuteninterupt);
	}
	
	if ((anzeige&(1<<refreshdisplay)))			//Anzeige aktualieseren
	{
		//Hier aktuelle seite neu ausgeben
		mon->draw();
		anzeige &= ~(1<<refreshdisplay);
	}
	/*
	if ((rtc.interupts&(1<<Weckeractiv)))
	{
		if ((rtc.interupts&(1<<Weckerein)))
		{
			anzeige |= (1<<blinkflag) | (1<<refreshdisplay);
			rtc.interupts &= ~(1<<Weckerein);
		}
	}*/
}

void uhreinstellen(){
	rtc.RTCstop();
	
	oled.clearFrame();
	char buffer[20];
	uint8_t buffersize;
	uint8_t pos=0;
	while(pos<11){
		buffersize=sprintf(buffer,"Uhreinstellung");
		for(uint8_t i = 0; i<buffersize;i++){
			oled.draw_ASCI(buffer[i],charsize*i,0);
		}
		buffersize=sprintf(buffer,"--------------");
		for(uint8_t i = 0; i<buffersize;i++){
			oled.draw_ASCI(buffer[i],charsize*i,8);
		}
		buffer[0]=rtc.Stunden/10;
		buffer[1]=rtc.Stunden%10;
		buffer[2]=':';
		buffer[3]=rtc.Minuten/10;
		buffer[4]=rtc.Minuten%10;
		buffer[5]=' ';
		buffer[6]=rtc.Tag/10;
		buffer[7]=rtc.Tag%10;
		buffer[8]='.';
		buffer[9]=rtc.Monat/10;
		buffer[10]=rtc.Monat%10;
		buffer[11]='.';
		buffer[12]=rtc.Jahr/10;
		buffer[13]=rtc.Jahr%10;
		buffersize=14;
		for(uint8_t i = 0; i<5;i++){
			if(i!=2){
				oled.draw_number16x16(buffer[i],i*numbersmalsize,2*charhighte);
			}
			else {
				oled.draw_ASCI(buffer[i],i*numbersmalsize+numbersmalsize/2,2*charhighte+charhighte/2);
			}
		}
		for(uint8_t i = 6; i<buffersize;i++){
			if(i!=8 && i!=11){
				oled.draw_number16x16(buffer[i],i*numbersmalsize-6*numbersmalsize,4*charhighte);
			}
			else {
				oled.draw_ASCI(buffer[i],i*numbersmalsize+numbersmalsize/2-6*numbersmalsize,5*charhighte);
			}
		}

		oled.sendFrame();
		oled.clearFrame();
		switch (pos) {
			case 0:
			rtc.Stunden=Tastatur.ZahlenausTastatur()*10;
			if(rtc.Stunden>=24){
				rtc.Stunden=0;
				pos=-1;
				buffersize=sprintf(buffer,"Stunden falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 1:
			rtc.Stunden+=Tastatur.ZahlenausTastatur();
			if(rtc.Stunden>=24){
				rtc.Stunden=0;
				pos=-1;
				buffersize=sprintf(buffer,"Stunden falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 2:
			rtc.Minuten=Tastatur.ZahlenausTastatur()*10;
			if(rtc.Minuten>=60){
				rtc.Minuten=0;
				pos=1;
				buffersize=sprintf(buffer,"Minuten falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 3:
			rtc.Minuten+=Tastatur.ZahlenausTastatur();
			if(rtc.Minuten>=60){
				rtc.Minuten=0;
				pos=1;
				buffersize=sprintf(buffer,"Minuten falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 4:
			rtc.Tag=Tastatur.ZahlenausTastatur()*10;
			if(rtc.Tag>=32){
				rtc.Tag=0;
				pos=3;
				buffersize=sprintf(buffer,"Tag falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 5:
			rtc.Tag+=Tastatur.ZahlenausTastatur();
			if(rtc.Tag>=32){
				rtc.Tag=0;
				pos=3;
				buffersize=sprintf(buffer,"Tag falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 6:
			rtc.Monat=Tastatur.ZahlenausTastatur()*10;
			if(rtc.Monat>=13){
				rtc.Monat=0;
				pos=5;
				buffersize=sprintf(buffer,"Monat falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 7:
			rtc.Monat+=Tastatur.ZahlenausTastatur();
			if(rtc.Monat>=13){
				rtc.Monat=0;
				pos=5;
				buffersize=sprintf(buffer,"Monat falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			if(rtc.Monat==4||rtc.Monat==6||rtc.Monat==9||rtc.Monat==11){
				if(rtc.Tag>=31){
					rtc.Tag=0;
					rtc.Monat=0;
					pos=3;
					buffersize=sprintf(buffer,"Tag falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						oled.draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
			}
			else if(rtc.Monat==2){
				if(rtc.Tag>=30){
					rtc.Tag=0;
					rtc.Monat=0;
					pos=3;
					buffersize=sprintf(buffer,"Tag falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						oled.draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
			}
			break;
			case 8:
			rtc.Jahr=Tastatur.ZahlenausTastatur()*10;
			if(rtc.Jahr>=50){
				rtc.Jahr=0;
				pos=7;
				buffersize=sprintf(buffer,"Jahr falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			break;
			case 9:
			rtc.Jahr+=Tastatur.ZahlenausTastatur();
			if(rtc.Jahr>=50){
				rtc.Jahr=0;
				pos=7;
				buffersize=sprintf(buffer,"Jahr falsch");
				for(uint8_t i = 0; i<buffersize;i++){
					oled.draw_ASCI(buffer[i],charsize*i,7*8);
				}
			}
			if(rtc.Monat==2){
				if(rtc.Jahr%4==0){
					if (rtc.Tag>=30)
					{
						rtc.Tag=0;
						rtc.Monat=0;
						rtc.Jahr=0;
						pos=3;
						buffersize=sprintf(buffer,"Tag falsch");
						for(uint8_t i = 0; i<buffersize;i++){
							oled.draw_ASCI(buffer[i],charsize*i,7*8);
						}
					}
				}
				else{
					if(rtc.Tag>=29){
						rtc.Tag=0;
						rtc.Monat=0;
						rtc.Jahr=0;
						pos=3;
						buffersize=sprintf(buffer,"Tag falsch");
						for(uint8_t i = 0; i<buffersize;i++){
							oled.draw_ASCI(buffer[i],charsize*i,7*8);
						}
					}
				}
			}
			break;
			default:
			pos++;
			break;
		}
		pos++;
	}
	
	rtc.Sekunden		= 0;
	rtc.HundSekunden	= 0;
	//speichern der neuen Zeit im EEPROM
	EEPROM_Write(EEMINUTEN,rtc.Minuten);
	EEPROM_Write(EESTUNDEN,rtc.Stunden);
	EEPROM_Write(EETAGE,rtc.Tag);
	EEPROM_Write(EEMONAT,rtc.Monat);
	EEPROM_Write(EEJAHR,rtc.Jahr);
	
	oled.clearFrame();
	rtc.ausgabedatumneu();
	rtc.ausgabezeitneu();
	rtc.RTCstart();
}
