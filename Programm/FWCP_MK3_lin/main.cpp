/*
 * FWCP_MK3.cpp
 *
 * Created: 28.11.2015 18:12:54
 * Author : Lüdemann
 */ 
#define VERSIONSNUMMER 3.09
#define SPANNUNGSTEILER 2.0069
#define F_CPU 8000000
#define BATMIN 3.6
#define BATMAX 4.2
#define zeitproachtzaehlungen 0.001024
#define zaehlungenprozeiteinheit 8.0
#define REEDMS 5
#define GMT 2

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

//nicht mehr verwendet
//#include "SOUND.h"
//SOUND Lautsprecher;

#include "LSM303D.h"
LSM303D Accelerometer;

#include "BMP180.h"
BMP180 druck;

#include "ADC.h"

#include "Output.h"
Output LED('B',PORTB1);
Output Vibm('B',PORTB2);
Output IRLED('D',PORTD7);

extern "C" {
	#include "ffconf.h"
	#include "diskio.h"
	#include "ff.h"
	};

//Anzeigebits
#define refreshdisplay  0
#define blinkflag       1

uint8_t anzeige;	//Flagregister fuer die Anziegenschaltung

#define updaterate		0
#define mounttingstat	1
#define loggingstat		2

uint8_t statusreg;
uint8_t position;		//Numer der Aktuellen Seite fuer Array und sonstieges

//Einbau der Festspeicherung von Einstellungswerten im EEPROM
#include "EEPROM.h"
//Ausgelagerte Sammlung der einzelnen Seitenlayouts
uint8_t FPS;
uint8_t FPScount;

//schlechte variante der Uhreinstellung
void uhreinstellen();

//SD karten Timer
uint8_t time0zahler;
ISR(TIMER0_COMPA_vect){
	time0zahler++;
	if (time0zahler%20==0)
	{
		statusreg |= (1<<updaterate);
	}
	disk_timerproc();	//Timer der SD Karte
}

ISR(TIMER2_OVF_vect){	//Vektor fuer die RTC
	//TCNT2=TIMER2RTCTIME;
	rtc.Sekunden++;
	rtc.interupts|= (1<<sekundeninterupt);
}

ISR(TIMER1_COMPA_vect){
	statusreg |= (1<<updaterate);
}

//GPS sachen
#define message				0
#define valid				1
#define complete			2
#define completenotvalid	3
#define notvalidgetdate		4
#define enable				6
#define fix					7
#define HOMELAT             53.224041
#define HOMELON             9.558916
uint8_t gpsstatus;
uint8_t gpsdata[72];
uint8_t gpscounter;
double lat;
double lon;
double dummy_place;
double dlat;
double dlon;
double gpsspeed;
uint8_t gpsstunde;
uint8_t gpsminute;
uint8_t gpssekunde;
uint8_t gpsTag;
uint8_t gpsMonat;
uint8_t gpsJahr;
void GPSENABLE()
{
	PORTC |=  (1<<PINC4);
	gpsstatus |= (1<<enable);
}
void GPSDISABLE()
{
	PORTC &= ~(1<<PINC4);
	gpsstatus &= ~((1<<enable) | (1<<fix));
}
double get_distance(double lat1, double lon1, double lat2, double lon2){
    double dist = 0;
    float radius_e = 6371e3;
    double phi1 = lat1*M_PI/180.;
    double phi2 = lat2*M_PI/180.;
    double dphi = phi2-phi1;
    double dlambda = lon2*M_PI/180.-lon1*M_PI/180.;

    double a = sin(dphi/2.)*sin(dphi/2.)+cos(phi1)*cos(phi2)*sin(dlambda/2.)*sin(dlambda/2.);
    double c = 2*atan2(sqrt(a),sqrt(1-a));
    return radius_e*c;
}
double get_bearing(double lat1, double lon1, double lat2, double lon2){
    double phi1 = lat1*M_PI/180.;
    double phi2 = lat2*M_PI/180.;
    double lam1 = lon1*M_PI/180.;
    double lam2 = lon2*M_PI/180.;

    double y = sin(lam2-lam1)*cos(phi2);
    double x =   cos(phi1)*sin(phi2) 
                -sin(phi1)*cos(phi2)*cos(lam2-lam1);

    double bearing = atan2(y,x)*180./M_PI;
    if(bearing<0){bearing=360+bearing;}
    return bearing;
}
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
			gpsstatus &= ~((1<<message) | (1<<notvalidgetdate));
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
void maininterupthandler(monitor *mon);
void tastaturhandler(monitor *mon, uint8_t taste);
void gpshandler();

FATFS FATFS_Obj;
FIL logger;

DWORD get_fattime (void)
{
	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)((2000+rtc.Jahr) - 1980) << 25)
	| ((DWORD)rtc.Monat << 21)
	| ((DWORD)rtc.Tag << 16)
	| ((DWORD)rtc.Stunden << 11)
	| ((DWORD)rtc.Minuten << 5)
	| ((DWORD)rtc.Sekunden >> 1);
}

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
	
	druck.bmp180_getcalibration();
	
	while (1){
		tastaturhandler(Folien[position],Tastatur.unified());
		maininterupthandler(Folien[position]);
		gpshandler();
    }
}

void initialisierung(){
	//GPS EN aktivieren	
	DDRC |= (1<<PINC4);
	//nullen der Flagregister
	anzeige=0;
	statusreg=0;
	position=0;
	FPS=0;
	FPScount=0;
	time0zahler=0;
	
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
    dlat = HOMELAT;
	lon = 0;
    dlon = HOMELON;
	gpsspeed = 0;
	gpsstunde = 0;
	gpsminute = 0;
	gpssekunde = 0;
	gpsTag = 0;
	gpsMonat = 0;
	gpsJahr = 0;
	//GPSDISABLE();
	GPSENABLE();
	
	//Wilkommensanzeige
	wilkommen wil(&oled,&rtc);
	wil.draw();
	for (uint8_t i=0; i<10;i++)
	{
		_delay_ms(50);
		_delay_ms(50);
	}
	//Zeit aus speicher
	rtc.Sekunden= 0;
	rtc.Minuten	= EEPROM_Read(EEMINUTEN);
	rtc.Stunden	= EEPROM_Read(EESTUNDEN);
	rtc.Tag		= EEPROM_Read(EETAGE);
	rtc.Monat	= EEPROM_Read(EEMONAT);
	rtc.Jahr	= EEPROM_Read(EEJAHR);
    rtc.WMinuten= EEPROM_Read(EEWECKMINUTEN);
    rtc.WStunden= EEPROM_Read(EEWECKSTUNDEN);
	rtc.ausgabedatumneu();
	rtc.RTCstart();
    //rtc.interupts |= (1<<Weckerein);
	
	//AD deaktivieren zum stromsparen
	ACSR |= (1<<ACD);
	
	//USART aktivieren jetzt nur hier zum testen
	UCSR0B |= (1<<RXEN0);
	
	//SD Karten timer init
	TCCR0A	 = (1<<WGM01);		//Timer im ctc Mode
	OCR0A	 = 38;		//ctc counter ende
	TIMSK0	|= (1<<OCIE0A);
	TCCR0B	|= (1<<CS02) | (1<<CS00);	//presc=1024
	
	sei();
}

void tastaturhandler(monitor *mon, uint8_t taste){
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
	            if ((rtc.interupts&(1<<Weckeractiv))){
                    Vibm.off();
                    anzeige &= ~(1<<blinkflag);
                    oled.invert(0);
                    rtc.interupts &= ~(1<<Weckeractiv);
                }
				break;
			default:
				//alle anderen Tasten werden an den jeweiligen Handler weiter gegeben
				mon->tastendruck(&taste);
				break;
		}
	}
}

void maininterupthandler(monitor *mon){
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
	//SD Karte mounten bzw. aufnahme starten
	else if (position==3)
	{
		if (mon->posy==3 && mon->posx==1)
		{
			if (!(statusreg&(1<<mounttingstat)))
			{
				//mounting sd Karte
				if (disk_initialize(0) == 0)
				{
					if (f_mount(&FATFS_Obj,"",0) == 0)
					{
						statusreg |= (1<<mounttingstat);
					}
				}
			}
			else if (!(statusreg&(1<<loggingstat)))
			{
				f_mount(0,"",0);
				statusreg &= ~(1<<mounttingstat);
			}
			mon->posx--;
		}
		else if (mon->posy==4 && mon->posx==1)
		{
			if (!(statusreg&(1<<loggingstat)) && (statusreg&(1<<mounttingstat)) )
			{
				if (disk_status(0) == 0)
				{
					char name[13];
					sprintf(name,"%02u%02u%02u%02u.txt",rtc.Monat,rtc.Tag,rtc.Stunden,rtc.Minuten);
					f_open(&logger, name, FA_OPEN_ALWAYS | FA_WRITE);
					//Zeit und GPS
					f_printf(&logger,"#Zeit [s]\tlongitude [1e6]\tLatitude [1e5]\tGPSSpeed [1e2 km/h] ");
					//Tacho
					f_printf(&logger,"\tTacho [1e2 km/h] ");
					//Barometer
					f_printf(&logger,"\tTemperatur [10 C] \tDruck [Pa] \tHoeheSee [10 m]");
					//Batterie
					f_printf(&logger,"\tBatterie[1e2 V]\n");
				}
				
				statusreg |= (1<<loggingstat);
			}
			else{
				f_sync(&logger);
				f_close(&logger);
				statusreg &= ~(1<<loggingstat);
			}
			mon->posx--;
		}
	}
	
	if (statusreg&(1<<updaterate))				//24 FPS fuer schnelle anzeigen
	{
		statusreg &= ~(1<<updaterate);
		anzeige |= (1<<refreshdisplay);
		FPScount++;
	}
	
	if ((rtc.interupts&(1<<sekundeninterupt)))	//Sekunden
	{
		rtc.zeit();
		druck.bmp180_getaltitude();
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
		
		if ((statusreg&(1<<mounttingstat)) && (statusreg&(1<<loggingstat)))
		{
			uint32_t Sekundenges = rtc.Stunden*3600;
			Sekundenges += rtc.Minuten*60;
			Sekundenges += rtc.Sekunden;
			//Zeit
			f_printf(&logger,"%02d.%02d.%02d_%02d:%02d:%02d",rtc.Tag,rtc.Monat,rtc.Jahr+2000,rtc.Stunden,rtc.Minuten,rtc.Sekunden);
			//GPS Daten und Zeit
			//f_printf(&logger,"%ld\t%ld\t%ld\t%ld\t",(uint32_t)Sekundenges,(int32_t)(lon*1000000),(int32_t)(lat*100000),(int32_t)(gpsspeed*100));
			f_printf(&logger,"\t%ld\t%ld\t%ld",(int32_t)(lon*1000000),(int32_t)(lat*100000),(int32_t)(gpsspeed*100));
			//Tacho
			f_printf(&logger,"\t%ld",(int32_t)(geschw*100));
			//Barometer
			f_printf(&logger,"\t%d\t%lu\t%d",(int16_t)(druck.temperature*10),(uint32_t)(druck.pressure*100),(int16_t)(druck.altitude*10));
			//Batterie
			f_printf(&logger,"\t%d\n",(int16_t)(batterie*100.0));
		}
		
		FPS=FPScount;
		FPScount=0;
		anzeige |= (1<<refreshdisplay);
		rtc.interupts &= ~(1<<sekundeninterupt);
	}
	if ((rtc.interupts&(1<<minuteninterupt)))		//Minuten
	{
		if ((statusreg&(1<<mounttingstat)) && (statusreg&(1<<loggingstat)))
		{
			f_sync(&logger);
		}
		anzeige |= (1<<refreshdisplay);
		rtc.interupts &= ~(1<<minuteninterupt);
	}
	
	if ((anzeige&(1<<refreshdisplay)))			//Anzeige aktualieseren
	{
		//Hier aktuelle seite neu ausgeben
		mon->draw();
		anzeige &= ~(1<<refreshdisplay);
        if ((anzeige&(1<<blinkflag)))			//Anzeige blinken
        {
            static uint8_t inver = 0;
            if (inver == 0){
                inver = 1;
                LED.on();
            }
            else{
                inver = 0;
                LED.off();
            }
            oled.invert(inver);
        }
	}
	
	if ((rtc.interupts&(1<<Weckeractiv)))
	{
		if ((rtc.interupts&(1<<Weckerein)))
		{
			anzeige |= (1<<blinkflag) | (1<<refreshdisplay);
            Vibm.on();
            LED.on();
			rtc.interupts &= ~(1<<Weckerein);
		}
	}
}

void gpshandler(){
	if ((gpsstatus&(1<<complete)))
	{
		if (!(gpsstatus&(1<<fix)))
		{
			gpsstatus |= (1<<fix);
		}
		double la	= 0;
		double lo	= 0;
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
		la =	(gpsdata[20] - '0')*10;
		la +=	(gpsdata[21] - '0');
		float lamin =	(gpsdata[22] - '0')*10;
		lamin +=		(gpsdata[23] - '0');
		lamin +=		(gpsdata[25] - '0')/10.0;
		lamin +=		(gpsdata[26] - '0')/100.0;
		lamin +=		(gpsdata[27] - '0')/1000.0;
		lamin +=		(gpsdata[28] - '0')/10000.0;
		la +=	lamin/60.0;
		if (gpsdata[30] != 'N')
		{
			la *= -1;
		}
		
		//Longitude
		lo =	(gpsdata[32] - '0')*100;
		lo +=	(gpsdata[33] - '0')*10;
		lo +=	(gpsdata[34] - '0');
		float lomin =	(gpsdata[35] - '0')*10;
		lomin +=		(gpsdata[36] - '0');
		lomin +=		(gpsdata[38] - '0')/10.0;
		lomin +=		(gpsdata[39] - '0')/100.0;
		lomin +=		(gpsdata[40] - '0')/1000.0;
		lomin +=		(gpsdata[41] - '0')/10000.0;
		lo +=			lomin/60.0;
		if (gpsdata[43] != 'E')
		{
			lo *= -1;
		}
		
		//Avvaragging
		//gpsmovingavv(la,lo);
        lat = la;
        lon = lo;
		
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
	else if ((gpsstatus&(1<<completenotvalid)))
	{
		if (gpsstatus&(1<<fix))
		{
			gpsstatus &= ~(1<<fix);
		}
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
