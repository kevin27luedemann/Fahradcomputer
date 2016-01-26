/*
 * FWCP_MK3.cpp
 *
 * Created: 28.11.2015 18:12:54
 * Author : Lüdemann
 */ 
#define VERSIONSNUMMER 3.02
#define SPANNUNGSTEILER 2.0069
#define F_CPU 8000000
#define BATMIN 3.6
#define zeitproachtzaehlungen 0.001024
#define zaehlungenprozeiteinheit 8.0
#define REEDMS 5
#define GMT 1

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>

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

//#include "Kompass.h"
//Kompass kompass;

#include "ADC.h"

#include "Output.h"
Output LED('B',PORTB1);
Output Vibrationsmotor('B',PORTB2);
Output IRLED('D',PORTD7);
Output Sound('B',PORTB3);

//Anzeigebits
#define refreshdisplay 0

uint8_t anzeige;	//Flagregister fuer die Anziegenschaltung

#define updaterate		0
#define powersavemode	1

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
	if (statusreg&(1<<powersavemode))
	{
		SMCR &= ~(1<<SE);
	}
}

ISR(TIMER1_COMPA_vect){
	statusreg |= (1<<updaterate);
}

#define message				0
#define valid				1
#define complete			2
#define completenotvalid	3
#define notvalidgetdate		4
#define fix					7
//GPS sachen
uint8_t gpsstatus;
uint8_t gpsdata[72];
uint8_t gpscounter;
double lat;
double lon;
double gpsspeed;
uint8_t gpsstunde;
uint8_t gpsminute;
uint8_t gpssekunde;
uint8_t gpsTag;
uint8_t gpsMonat;
uint8_t gpsJahr;

ISR(USART0_RX_vect){
	uint8_t temp = UDR0;
	if (temp == '$' && !(gpsstatus&(1<<complete)) && !(gpsstatus&(1<<completenotvalid)))
	{
		gpscounter = 0;
		gpsstatus |= (1<<message);
	}
	if ((gpsstatus&(1<<message)))
	{
		gpsdata[gpscounter] = temp;
		gpscounter++;
		if (gpscounter>72)
		{
			gpsstatus &= ~(1<<message);
		}
		else if (gpscounter==19)
		{
			if (gpsdata[4]=='M' && gpsdata[5]=='C')
			{
				if (gpsdata[18]!='A')
				{
					gpsstatus &= ~((1<<valid));
					gpsstatus |= (1<<notvalidgetdate);
				}
				else{
					gpsstatus |= (1<<valid);
				}
			}
			else{
				gpsstatus &= ~(1<<valid);
				gpsstatus &= ~(1<<message);
			}
		}
		else if (gpscounter == 42 && (gpsstatus&(1<<notvalidgetdate)))
		{
			gpsstatus |= (1<<completenotvalid);
			gpsstatus &= ~(1<<message);
		}
		else if (gpscounter == 70 && (gpsstatus&(1<<valid)))
		{
			gpsstatus |= (1<<complete);
			gpsstatus &= ~((1<<message) | (1<<valid));
		}
	}
}


//Tacho funktionen
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
#define numberofpages 5
#include "Monitor.h"

void initialisierung();
void maininterupthandler(monitor *mon, uint8_t taste);
void gpshandler();

int main(void)
{
    initialisierung();
    monitor* Folien[numberofpages+1] =
	{
		new uhr(&oled,&rtc),
		new tacho(&oled,&rtc),
		new wandern(&oled,&rtc),
		new einstellungen(&oled,&rtc),
		new offscreen(&oled,&rtc),
		new menue(&oled,&rtc)
	};
	
	

	while (1) 
    {
		maininterupthandler(Folien[position],Tastatur.unified());
		gpshandler();
		if (statusreg&(1<<powersavemode))
		{
			SMCR |= (1<<SE);
		}
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
	
	//USART initalisieren, aktivieren erst spaeter
	PIND &= ~((1<<PIND1) | (1<<PIND2));
	DDRD &= ~((1<<PIND1) | (1<<PIND2));
	UBRR0H = 0;
	UBRR0L = 51;							//9600 Baud
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);	// 8Bit Frame
	UCSR0B = (1<<RXCIE0);
	gpscounter = 0;
	gpsstatus = (1<<fix);
	lat = 0;
	lon = 0;
	gpsspeed = 0;
	gpsstunde = 0;
	gpsminute = 0;
	gpssekunde = 0;
	gpsTag = 0;
	gpsMonat = 0;
	gpsJahr = 0;
	
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
	
	//Sleepmode Grundeinstelung
	SMCR |= (1<<SM1) | (1<<SM0);
	
	//AD deaktivieren zum stromsparen
	ACSR |= (1<<ACD);
	
	//USART aktivieren jetzt nur hier zum testen
	UCSR0B |= (1<<RXEN0);
	
	sei();
}

void maininterupthandler(monitor *mon, uint8_t taste){
	if (taste!=' ')
	{
		anzeige |= (1<<refreshdisplay);
		switch (taste)							//Tastendruck ueberpruefen
		{
			case 'm':
				position = numberofpages;
				break;
			case 'l':
				if (mon->posx==0)
				{
					position = numberofpages;
				}
				else{
					mon->posx--;
				}
				break;
			case 'r':
				if (position==numberofpages)
				{
					position=mon->posy;
				}
				else if (!(mon->posx >= mon->maxentriesx))
				{
					mon->posx++;
				}
				break;
				case 'o':
					if (position==numberofpages)
					{
						position=mon->posy;
					}
					else if (!(mon->posx >= mon->maxentriesx))
					{
						mon->posx++;
					}
					break;
			case 'd':
				if (!(mon->posy >= mon->maxentries-1))
				{
					mon->posy++;
				}
				else{
					mon->posy = 0;
				}
				break;
			case 'u':
				if (!(mon->posy == 0))
				{
					mon->posy--;
				}
				else{
					mon->posy=mon->maxentries-1;
				}
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
				break;
		}
		if (position==0 || position==numberofpages)
		{
			statusreg |= (1<<powersavemode);
		}
		else{
			statusreg &= ~(1<<powersavemode);
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

void gpshandler(){
	if ((gpsstatus&(1<<complete)) && (gpsstatus&(1<<fix)))
	{
		//brechnung von Latitutde, Longitude, Zeit und Datum
		//Zeit
		gpsstunde =		(gpsdata[7] - '0')*10;
		gpsstunde +=	(gpsdata[8] - '0');
		gpsstunde += GMT;
		gpsminute =		(gpsdata[9] - '0')*10;
		gpsminute +=	(gpsdata[10] - '0');
		gpssekunde =	(gpsdata[11] - '0')*10;
		gpssekunde +=	(gpsdata[12] - '0');
		
		//Latitude
		lat =	(gpsdata[20] - '0')*10;
		lat +=	(gpsdata[21] - '0');
		float latmin =	(gpsdata[22] - '0')*10;
		latmin +=		(gpsdata[23] - '0');
		latmin +=		(gpsdata[25] - '0')/10.0;
		latmin +=		(gpsdata[26] - '0')/100.0;
		latmin +=		(gpsdata[27] - '0')/1000.0;
		latmin +=		(gpsdata[28] - '0')/10000.0;
		lat +=	latmin/60.0;
		if (gpsdata[30] != 'N')
		{
			lat *= -1;
		}
		
		//Longitude
		lon =	(gpsdata[32] - '0')*100;
		lon +=	(gpsdata[33] - '0')*10;
		lon +=	(gpsdata[34] - '0');
		float lonmin =	(gpsdata[35] - '0')*10;
		lonmin +=		(gpsdata[36] - '0');
		lonmin +=		(gpsdata[38] - '0')/10.0;
		lonmin +=		(gpsdata[39] - '0')/100.0;
		lonmin +=		(gpsdata[40] - '0')/1000.0;
		lonmin +=		(gpsdata[41] - '0')/10000.0;
		lon +=			lonmin/60.0;
		if (gpsdata[43] != 'E')
		{
			lon *= -1;
		}
		//Speed 
		volatile uint8_t counter = 45;
		uint8_t weiter = true;
		while (weiter)
		{
			if (gpsdata[counter]=='.')
			{
				weiter=false;
				counter--;
			}
			counter++;
		}
		
		gpsspeed = 0;
		for(uint8_t i=45;i<counter;i++){
			gpsspeed += (gpsdata[i]-'0')*pow(10,(counter-i-1));
		}
		gpsspeed += (gpsdata[counter+1]-'0')*0.1;
		gpsspeed += (gpsdata[counter+2]-'0')*0.01;
		//umrechnen knoten in kmh
		gpsspeed *= 1.852;
		
		//date
		counter =19;
		volatile uint8_t nichterreicht = 0;
		
		while (nichterreicht < 7)
		{
			if (gpsdata[counter] == ',')
			{
				nichterreicht++;
			}
			counter++;
		}
		
		//Datum
		gpsTag =	(gpsdata[counter+0] - '0')*10;
		gpsTag +=	(gpsdata[counter+1] - '0');
		gpsMonat =	(gpsdata[counter+2] - '0')*10;
		gpsMonat +=	(gpsdata[counter+3] - '0');
		gpsJahr =	(gpsdata[counter+4] - '0')*10;
		gpsJahr +=	(gpsdata[counter+5] - '0');
		
		gpsstatus &= ~(1<<complete);
	}
	else if ((gpsstatus&(1<<completenotvalid))  && (gpsstatus&(1<<fix)))
	{
		//Zeit
		gpsstunde =		(gpsdata[7] - '0')*10;
		gpsstunde +=	(gpsdata[8] - '0');
		gpsstunde += GMT;
		gpsminute =		(gpsdata[9] - '0')*10;
		gpsminute +=	(gpsdata[10] - '0');
		gpssekunde =	(gpsdata[11] - '0')*10;
		gpssekunde +=	(gpsdata[12] - '0');
		
		volatile uint8_t counter = 19;
		volatile uint8_t nichterreicht = 0;
		
		while (nichterreicht < 7)
		{
			if (gpsdata[counter] == ',')
			{
				nichterreicht++;
			}
			counter++;
		}
		
		//Datum
		gpsTag =	(gpsdata[counter+0] - '0')*10;
		gpsTag +=	(gpsdata[counter+1] - '0');
		gpsMonat =	(gpsdata[counter+2] - '0')*10;
		gpsMonat +=	(gpsdata[counter+3] - '0');
		gpsJahr =	(gpsdata[counter+4] - '0')*10;
		gpsJahr +=	(gpsdata[counter+5] - '0');
		
		gpsstatus &= ~(1<<completenotvalid);
	}
	//fix status pruefen, wenn implementiert
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
