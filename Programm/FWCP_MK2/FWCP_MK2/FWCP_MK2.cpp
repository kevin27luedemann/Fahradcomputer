/*
 * FWCP_MK2.cpp
 *
 * Created: 08.10.2015 08:41:30
 *  Author: Lüdemann
 */
#define VERSIONSNUMMER 2.10
#define SPANNUNGSTEILER 2.0069
#define F_CPU 8000000
#define BATMIN 3.6

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "Kompass.h"
Kompass kompass;

#include "RTC.h"
RTC rtc;

#include "Display.h"
Display oled;

#include "Interface.h"
Interface Tastatur;

#include "Output.h"
Output LED('B',PORTB1);
Output Vibrationsmotor('B',PORTB2);
Output IRLED('D',PORTD7);
Output Sound('B',PORTB3);

#include "LSM303D.h"
LSM303D Accelerometer;

//#include "Pressure.h"
//Pressure Baro;

#include "SOUND.h"
SOUND Lautsprecher;

#include "ADC.h"

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

//Einbau der Festspeicherung von Einstellungswerten im EEPROM
#include "EEPROM.h"
//Ausgelagerte Sammlung der einzelnen Seitenlayouts
uint8_t FPS;
#include "Seiten.h"

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
	statusreg=0;
	pos=0;
	FPS=0;
	//initialisieren des Zaehler fuer die Winkelgeschw sowie den Timer
	geschw=0;
	strecke = 0;
	maxgeschw = 0;
	Fahrtzeit = 0;
	inittimer(0);
	//Tastatur inm Contruktor initialisiert
	//Eingang fuer den Reedkontak schalten mit internem Pullup
	DDRA |= ((1<<PORTA6));
	PORTA |= (1<<PORTA6);
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
	while (ADCSRA & (1<<ADSC) ) {}
	(void) ADCW;
	//Wilkommensanzeige
	willkommenpage();
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

/*
formel: f=f_CPU/(2*N*(1+OCR1A))
		N*(1+OCR1A)	= f_CPU/(f*2)
					= 8000000/(24*2)
					= 166667
		=> N=64 bei OCR1A=2604-1 bei f=24
		=> CS11 und CS10
*/
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

void maininterupthandler(){
	//interupt dierekt aus der rtc
	//if((rtc.interupts & (1<<hundinterupt))){
	//rtc.zeit();
	//rtc.interupts&=~(1<<hundinterupt);
	//}
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
}

void anzeigehandler(){
	//Handler fuer 1Hz Flag
	if ((statusreg&(1<<updaterate)))
	{
		FPS++;
		if ((anzeige&(1<<Kompasflag)))
		{
			Accelerometer.readacc();
			anzeige_kompass(kompass.angle(Accelerometer.roll,Accelerometer.pitch));
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Fahradflag)))
		{
			Accelerometer.readacc();
			fahradschirm(geschw,kompass.angle(Accelerometer.roll,Accelerometer.pitch),strecke,maxgeschw, Fahrtzeit);
			//fahradschirm(geschw,kompass.angle(0,0),strecke,maxgeschw, Fahrtzeit);
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Wanderflag))&&!(anzeige&(1<<Einstellungsflag)))
		{
			Wanderseite();
			anzeige |= (1<<refreshdisplay);
		}
		statusreg &= ~(1<<updaterate);
	}
	if ((rtc.interupts & (1<<sekundeninterupt)))
	{
		rtc.zeit();
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
			Accelerometer.readacc();
			anzeige_kompass(kompass.angle(Accelerometer.roll,Accelerometer.pitch));
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Fahradflag)))
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
			Accelerometer.readacc();
			fahradschirm(geschw,kompass.angle(Accelerometer.roll,Accelerometer.pitch),strecke,maxgeschw, Fahrtzeit);
			//fahradschirm(geschw,kompass.angle(0,0),strecke,maxgeschw, Fahrtzeit);
			//Fuer Testzwecke entfernt um neue Methode zu testen, siehe Oben
			//if (rtc.Sekunden%2)
			//{
				//geschw=0;
			//}
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Stoppuhrflag)))
		{
			Stoppuhrseite();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Timerflag)))
		{
			if (rtc.interupts&(1<<Alarm))
			{
				anzeige |= (1<<blinkflag);
				rtc.interupts &= ~(1<<Alarm);
			}
			timerseite();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Druckflag)))
		{
			Pressuresensor();
			anzeige |= (1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Einstellungsflag)&&(anzeige&(1<<Wanderflag))))
		{
			if (pos<7)
			{
				pos--;
			}
			if (pos<6)
			{
				Accelerometer.get_gravity();
			}
			if (pos==0)
			{
				anzeige &= ~(1<<Einstellungsflag);
			}
			find_gravity();
			anzeige |= (1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Wanderflag)))
		{
			Accelerometer.schritt(rtc.Sekunden%2);
			Wanderseite();
			anzeige |= (1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Weckeranzeigeflag))&&(anzeige&(1<<Einstellungsflag)))
		{
			Weckeranzeige();
			anzeige|=(1<<refreshdisplay);
		}
		if ((anzeige&(1<<blinkflag)))
		{
			oled.invert(rtc.Sekunden%2);
			LED.toggle();
			Vibrationsmotor.on();
		}
		FPS=0;
		rtc.interupts&=~(1<<sekundeninterupt);
	}
	//Minuteninterrupt
	if ((rtc.interupts&(1<<minuteninterupt)))
	{
		if ((anzeige&(1<<Uhrflaggross)))
		{
			uhranzeigenmin();
			anzeige|=(1<<refreshdisplay);
		}
		else if ((anzeige&(1<<Weckeranzeigeflag)))
		{
			Weckeranzeige();
			anzeige|=(1<<refreshdisplay);
		}
		rtc.interupts&=~(1<<minuteninterupt);
	}
	//neuen Framebuffer an das Dispay senden
	if ((anzeige&(1<<refreshdisplay)))
	{
		oled.sendFrame();
		anzeige&=~(1<<refreshdisplay);
	}
}

void eingabehandler(uint8_t taste){
	if (taste!='A')
	{
		rtc.interupts |= (1<<sekundeninterupt);
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
				anzeige|=(1<<refreshdisplay) | (1<<Stoppuhrflag);
				break;
				
				case '3':
				//Starten der Timer APP	
				anzeige|=(1<<Timerflag) | (1<<Einstellungsflag);
				break;
				
				case '4':
				rtc.interupts |=(1<<minuteninterupt);
				anzeige |= (1<<Weckeranzeigeflag);
				break;
				
				case '5':
				//Wechseln auf grosse Uhr
				rtc.interupts |=(1<<minuteninterupt);
				anzeige |=(1<<Uhrflaggross) | (1<<refreshdisplay);
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
				rtc.interupts |= (1<<minuteninterupt);
				anzeige|=(1<<Uhrflaggross);
				break;
				case '4':
				anzeige |= (1<<Einstellungsflag) | (1<<Weckeranzeigeflag);
				break;
				case '5':
				Accelerometer.ACCStreammode();
				pos=7;
				anzeige |= (1<<Einstellungsflag) | (1<<Wanderflag);
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
				//starten des Timers fuer die geschwindigkeit
				inittimer(1);
				break;
				
				case '2':
				//Wanderanzeige schalten
				Accelerometer.ACCStreammode();
				anzeige|=(1<<refreshdisplay) | (1<<Wanderflag);
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
				case '7':
				anzeige |= (1<<Druckflag);
				break;
				default:
				//menueflag erneut setzen um abschalten zu verhindern
				anzeige|=(1<<menueflag);
				break;
			}
		}
		else if ((anzeige&(1<<Einstellungsflag))&&(anzeige&(1<<Weckeranzeigeflag)))
		{
			switch (pos)
			{
				case 0:
				rtc.WStunden=0;
				rtc.WMinuten=0;
				rtc.interupts &= ~((1<<Weckerein)|(1<<Weckeractiv));
				switch (taste)
				{
					case '#':
					break;
					case '*':
					break;
					default:
					rtc.WStunden=(taste-'0')*10;
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
					rtc.WStunden+=(taste-'0');
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
					rtc.WMinuten=(taste-'0')*10;
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
					rtc.WMinuten+=(taste-'0');
					pos++;
					break;
				}
				pos=0;
				if (!(rtc.WStunden>=24) && !(rtc.WMinuten>=60))
				{
					anzeige&=~(1<<Einstellungsflag);
					rtc.interupts|=(1<<Weckerein);
					rtc.interupts|=(1<<minuteninterupt);
				}
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
							pos=0;
							rtc.Timerzahler=0;
							break;
						case '*':
							pos=0;
							rtc.Timerzahler++;
							anzeige&=~(1<<Einstellungsflag);
							rtc.interupts|=(1<<Timerlauft);
							break;
						default:
						rtc.Timerzahler=(taste-'0')*10;
						pos++;
						break;
					}
					break;
				case 1:
					switch (taste)
					{
						case '#':
							pos=0;
							rtc.Timerzahler=0;
							break;
						case '*':
							pos=0;
							rtc.Timerzahler++;
							anzeige&=~(1<<Einstellungsflag);
							rtc.interupts|=(1<<Timerlauft);
							break;
						default:
						rtc.Timerzahler+=(taste-'0');
						pos++;
						break;
					}
					break;
				case 2:
					switch (taste)
					{
						case '#':
							pos=0;
							rtc.Timerzahler=0;
							break;
						case '*':
							pos=0;
							rtc.Timerzahler++;
							anzeige&=~(1<<Einstellungsflag);
							rtc.interupts|=(1<<Timerlauft);;
							break;
						default:
						rtc.Timerzahler+=(taste-'0')*10*60;
						pos++;
						break;
					}
					break;
				case 3:
					switch (taste)
					{
						case '#':
							pos=0;
							rtc.Timerzahler=0;
							break;
						case '*':
							pos=0;
							rtc.Timerzahler++;
							anzeige&=~(1<<Einstellungsflag);
							rtc.interupts|=(1<<Timerlauft);
							break;
						default:
						rtc.Timerzahler+=(taste-'0')*60;
						pos++;
						break;
					}
					break;
				case 4:
					switch (taste)
					{
						case '#':
							pos=0;
							rtc.Timerzahler=0;
							break;
						case '*':
							pos=0;
							rtc.Timerzahler++;
							anzeige&=~(1<<Einstellungsflag);
							rtc.interupts|=(1<<Timerlauft);
							break;
						default:
						rtc.Timerzahler+=(taste-'0')*10*3600;
						pos++;
						break;
					}
					break;
				case 5:
					switch (taste)
					{
						case '#':
							pos=0;
							rtc.Timerzahler=0;
							break;
						case '*':
							pos=0;
							rtc.Timerzahler++;
							anzeige&=~(1<<Einstellungsflag);
							rtc.interupts|=(1<<Timerlauft);
							break;
						default:
						rtc.Timerzahler+=(taste-'0')*3600;
						pos++;
						break;
					}
					pos=0;
					rtc.Timerzahler++;
					anzeige&=~(1<<Einstellungsflag);
					rtc.interupts|=(1<<Timerlauft);
					break;
				default:
					break;
			}
		}
		else if (((anzeige&(1<<Einstellungsflag))&&(anzeige&(1<<Wanderflag))))
		{
			if (pos==7)
			{
				if (taste == '*')
				{
					pos=6;
				}
			}
			else{
				pos=6;
			}
		}
		else if ((anzeige&(1<<Stoppuhrflag)))
		{
			if (taste=='*')
			{
				if ((rtc.interupts&(1<<Stoppuhrlauft)))
				{
					rtc.interupts&=~(1<<Stoppuhrlauft);
				}
				else{
					rtc.interupts|=(1<<Stoppuhrlauft);
				}
			}
			else if (taste=='0')
			{
				rtc.Stoppuhrzahler=0;
			}
			else if (taste=='#')
			{
				anzeige&=~((1<<Stoppuhrflag));
				rtc.interupts&=~(1<<Stoppuhrlauft);
				anzeige|=(1<<menueflag);
			}
		}
		else if ((anzeige&(1<<Uhrflag)))
		{
			if (taste=='#')
			{
				pos=0;
				anzeige&=~(1<<Uhrflag);
				anzeige|=(1<<menueflag);
			}
			else if (taste=='*')
			{
				pos++;
				if (pos>1)
				{
					pos=0;
				}
			}
			else if (taste=='0')
			{
				if (LED.ison())
				{
					LED.off();
				}
				else{
					LED.on();
				}
			}
		}
		else if ((anzeige&(1<<Kompasflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Kompasflag);
				anzeige|=(1<<menueflag);
			}
			else if (taste=='0')
			{
				if (LED.ison())
				{
					LED.off();
				}
				else{
					LED.on();
				}
			}
		}
		else if ((anzeige&(1<<Fahradflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Fahradflag);
				anzeige|=(1<<menueflag);
				inittimer(2);
			}
			else if (taste=='*')
			{
				maxgeschw = 0;
				strecke = 0;
				Fahrtzeit = 0;
			}
			else if (taste=='0')
			{
				if (LED.ison())
				{
					LED.off();
				}
				else{
					LED.on();
				}
			}
		}
		else if ((anzeige&(1<<Druckflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Druckflag);
				anzeige|=(1<<menueflag);
			}
			else if (taste=='*')
			{
				//Baro.set_Pressure0(Baro.Press);
			}
			else if (taste=='0')
			{
				if (LED.ison())
				{
					LED.off();
				}
				else{
					LED.on();
				}
			}
		}
		else if ((anzeige&(1<<Timerflag)))
		{
			if (taste=='#')
			{
				anzeige&=~((1<<Timerflag)|(1<<blinkflag));
				rtc.interupts&=~((1<<Timerlauft)|(1<<Alarm));
				oled.invert(0);	//fuer den Fall, dass es invertiert blieb (50% der Faelle)
				LED.off();
				Vibrationsmotor.off();
				
				anzeige|=(1<<menueflag);
			}
		}
		else if ((anzeige&(1<<Uhrflaggross)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Uhrflaggross);
				anzeige|=(1<<menueflag);
			}
			else if (taste=='0')
			{
				if (LED.ison())
				{
					LED.off();
					Lautsprecher.off();
				}
				else{
					LED.on();
					Lautsprecher.on();
				}
			}
		}
		else if ((anzeige&(1<<Wanderflag)))
		{
			if (taste=='#')
			{
				anzeige &= ~(1<<Wanderflag);
				anzeige |= (1<<menueflag);
				Accelerometer.ACCBypassmode();
			}
		}
		else if ((anzeige&(1<<Weckeranzeigeflag)))
		{
			if (taste=='#')
			{
				anzeige&=~(1<<Weckeranzeigeflag);
				anzeige|=(1<<menueflag);
			}
			if (taste=='*')
			{
				if (rtc.interupts&(1<<Weckeractiv))
				{
					rtc.interupts &= ~((1<<Weckerein) | (1<<Weckeractiv));
				}
				else if ((rtc.interupts&(1<<Weckerein)))
				{
					rtc.interupts &= ~(1<<Weckerein);
				}
				else{
					rtc.interupts |= (1<<Weckerein);
				}
				rtc.interupts|=(1<<minuteninterupt);
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
				LED.off();
				Vibrationsmotor.off();
				if ((rtc.interupts&(1<<Weckeractiv)))
				{
					if ((rtc.interupts&(1<<Weckerein)))
					{
						rtc.interupts &= ~((1<<Weckerein)|(1<<Weckeractiv));
						rtc.interupts &= ~(1<<Alarm);
					}
				}
			}
		}
	}
}
