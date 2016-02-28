/*
 * Monitor.h
 *
 * Created: 13.12.2015 13:21:12
 *  Author: Lüdemann
 */ 


#ifndef MONITOR_H_
#define MONITOR_H_
#include <stdio.h>
#include <stdlib.h>
#include "Display.h"

#ifndef numberofpages
#define numberofpages 4
#endif

class monitor
{
	private:
	
	protected:
		char buffer[20];
		uint8_t buffersize;
		Display *oled;
		RTC *rtc;
		#define namesize 10
		char name[10];
	public:
		uint8_t posy;
		uint8_t posx;
		uint8_t maxentries;
		uint8_t maxentriesx;
	monitor(Display *ol, RTC *rt)
	{
		buffersize = 0;
		oled = ol;
		rtc = rt;
		posy=0;
		posx=0;
		maxentriesx = 1;
	}
	
	//getter Funktion fuer name
	char* getname(){
		return &name[0];
	}
	
	//draw header and overload it
	void header(){
		for(uint8_t i=0;i<8;i++){
			oled->draw_ASCI(rtc->msg_uhr[i],i*charsize,0);
		}
		buffersize=sprintf(buffer,"Bat: %i%%",Batteriestatus());
		for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
			oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
		}
	}
	void header(uint8_t page){
		switch (page) {
			case 0:	//standard Header fuer fast alle Pages
				for(uint8_t i=0;i<8;i++){
					oled->draw_ASCI(rtc->msg_uhr[i],i*charsize,0);
				}
				buffersize=sprintf(buffer,"Bat: %i%%",Batteriestatus());
				for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
					oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
				}
				break;
			case 1:	//standard Header fuer fast alle Pages
				for(uint8_t i=0;i<(bitsderrtc-3);i++){oled->draw_ASCI(rtc->msg_uhr[i],65+i*charsize,0*charhighte);}
				buffersize=sprintf(buffer,"%i%%",Batteriestatus());
				for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
					oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
				}
				break;
			case 2:	//standard Header fuer fast alle Pages
				buffersize=sprintf(buffer,"Bat.: %i%%",Batteriestatus());
				for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
					oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
				}
				break;
			default:
				break;
		}
	}
	
	//draw bottom and overload it
	void bottom(){
		for(uint8_t i=0;i<8;i++){
			oled->draw_ASCI(rtc->msg_dat[i],i*charsize+(SSD1306_WIDTH-bitsderrtc*charsize),7*charhighte);
		}
	}
	void bottom(uint8_t page){
		switch (page) {
			case 0:	//standard Header fuer fast alle Pages
				for(uint8_t i=0;i<8;i++){
					oled->draw_ASCI(rtc->msg_dat[i],i*charsize+(SSD1306_WIDTH-bitsderrtc*charsize),7*charhighte);
				}
				break;
			case 1:
				for(uint8_t i=0;i<8;i++){
					oled->draw_ASCI(rtc->msg_dat[i],5+i*charsize,7*charhighte);
				}
				break;
			case 2:
				for(uint8_t i=0;i<bitsderrtc;i++){oled->draw_ASCI(rtc->msg_dat[i],65+i*charsize,7*charhighte);}
				break;
			default:
				break;
		}
	}
	
	//taster pruef Funktion
	virtual uint8_t tastendruck(uint8_t *tast){
		return 0;
	}
	
	//drawfunktion
	virtual void draw(){
		oled->clearFrame();
	}

	//send the frame to the display
	void send(){
		oled->sendFrame();	
	}
};

class uhr:public monitor
{
	private:
	uint8_t Zeiger;
	public:
	uhr(Display *ol, RTC *rt):monitor(ol,rt)
	{
		char na[] = "Uhr";
		for(uint8_t i =0; i< namesize;i++){
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
		}
		Zeiger = 0;
		maxentries = 3;
	}
	
	//Zeiger setter und getter Funktion
	void setzeiger(uint8_t zeig){
		Zeiger = zeig;
	}
	uint8_t getZeiger() const{
		return Zeiger;
	}
	
	//Taster ueberpruefen
	uint8_t tastendruck(uint8_t *tast){
		if (*tast=='e')
		{
			if (Zeiger==1)
			{
				setzeiger(0);
			}
			else{
				setzeiger(1);
			}
		}
	return 0;
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header(1);
		bottom(2);

		oled->analog(rtc->Stunden,rtc->Minuten,rtc->Sekunden,Zeiger);
		//draw large number
		oled->draw_number16x16(rtc->msg_uhr[0]-'0',70,1.66*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[1]-'0',70+numbersmalsize,1.66*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[3]-'0',70,2.33*charhighte+numbersmalhight);
		oled->draw_number16x16(rtc->msg_uhr[4]-'0',70+numbersmalsize,2.33*charhighte+numbersmalhight);

		send();
	}
};

class tacho: public monitor
{
	private:
		
	public:
	tacho(Display *ol,RTC *rtc):monitor(ol,rtc)
	{
		char na[] = "Tacho";
		for(uint8_t i =0; i< namesize;i++){
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
		}
		nullen();
	}

	//Tasterhandler
	uint8_t tastendruck(uint8_t *tast){
		if (*tast=='e')
		{
			nullen();
		}
		return 0;
	}

	//Anzeige schalten
	void draw(){
		monitor::draw();
		header(2);
		bottom();

		//Rahmen zeichnen
		oled->drawHLine(0,SSD1306_HEIGHT-9,SSD1306_WIDTH);
		oled->drawHLine(0,numbersmalhight-1,SSD1306_WIDTH/2+5);
		oled->drawHLine(SSD1306_WIDTH/2+5,charhighte-1,SSD1306_WIDTH/2-5);
		oled->drawVLine(SSD1306_WIDTH/2+5,0,numbersmalhight);
		oled->drawHLine(SSD1306_WIDTH-4*numbersmalsize-1,5*charhighte-1,4*numbersmalsize+1);
		oled->drawVLine(SSD1306_WIDTH-4*numbersmalsize-1,numbersmalhight,SSD1306_HEIGHT-numbersmalhight);
		oled->drawHLine(SSD1306_WIDTH-4*numbersmalsize-1,3*charhighte,4*numbersmalsize+1);

		//lesbare Uhr
		oled->draw_number16x16(rtc->msg_uhr[0]-'0',0*numbersmalsize,0*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[1]-'0',1*numbersmalsize,0*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[3]-'0',2.33*numbersmalsize,0*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[4]-'0',3.33*numbersmalsize,0*charhighte);

		//Ausgabe der Geschwidigkeit
		buffersize=sprintf(buffer,"%3.1f",geschw);
		for(uint8_t i=((SSD1306_WIDTH/numbersmalsize)-buffersize);i<((SSD1306_WIDTH/numbersmalsize));i++){
			if(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize)]=='.'){
				oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize)],i*numbersmalsize,4*charhighte);
			}
			else{
				oled->draw_number16x16(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize)]-'0',i*numbersmalsize,3*charhighte);
			}
		}

		//Nadel erst, wenn Kompass implementiert
		//Anzeige der Richtung als Alternative
		//anzeige_kleinenadel(31,31+8,angle);

		//anzeige der gesammtstrecke
		buffersize=sprintf(buffer,"%.3fkm",strecke/1000);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+70,2*charhighte);}

		//anzeige der max geschwindigkeit
		buffersize=sprintf(buffer,"%.1fkm/h",maxgeschw);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+70,5*charhighte);}

		//anzeige der Fahrtzeit
		buffersize=sprintf(buffer,"%02i:%02i:%02i",uint8_t(Fahrtzeit/3600),uint8_t(Fahrtzeit/60),uint8_t(Fahrtzeit%60));
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+70,6*charhighte);}
		
		//anzeige der durschschnittsgeschw
		if (Fahrtzeit==0)
		{
			buffersize=sprintf(buffer,"%.1fkm/h",0.0);
		}
		else {
			buffersize=sprintf(buffer,"%.1fkm/h",(strecke/Fahrtzeit)*3.6);
		}
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+75,1*charhighte);}

		send();
	}

};

class wandern: public monitor
{
	private:
	
	public:	
	wandern(Display *ol,RTC *rtc):monitor(ol,rtc)
	{
		char na[] = "Wandern";
		for(uint8_t i =0; i< namesize;i++){
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
		}
	}
	
	uint8_t tastendruck(uint8_t *tast){
		if (*tast=='e')
		{
			rtc->Stunden	= gpsstunde;
			rtc->Minuten	= gpsminute;
			rtc->Sekunden	= gpssekunde;
			
			rtc->Tag		= gpsTag;
			rtc->Monat		= gpsMonat;
			rtc->Jahr		= gpsJahr;
			rtc->ausgabedatumneu();
			//speichern der neuen Zeit im EEPROM
			EEPROM_Write(EEMINUTEN,rtc->Minuten);
			EEPROM_Write(EESTUNDEN,rtc->Stunden);
			EEPROM_Write(EETAGE,rtc->Tag);
			EEPROM_Write(EEMONAT,rtc->Monat);
			EEPROM_Write(EEJAHR,rtc->Jahr);
		}
		else if (*tast=='A')
		{
			GPSENABLE();
		}
		else if (*tast=='B')
		{
			GPSDISABLE();
		}
		return 0;
	}
	
	void draw(){
		monitor::draw();
		header();
		bottom();
		/*
		//Debug data
		buffersize=sprintf(buffer,"Status: %i",gpsstatus);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,1*charhighte);}
		buffersize=sprintf(buffer,"Data: %c%c%c%c%c",gpsdata[0],gpsdata[1],gpsdata[2],gpsdata[3],gpsdata[4]);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,3*charhighte);}
		*/

		//Latitude
		buffersize=sprintf(buffer,"Lat: %.7f",lat);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
		
		//Longitude
		buffersize=sprintf(buffer,"Lon: %.7f",lon);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,3*charhighte);}
			
		//Speed
		buffersize=sprintf(buffer,"Spe: %.2f",gpsspeed);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
			
		//gpstime
		buffersize=sprintf(buffer,"%02i:%02i:%02i %02i.%02i.%02i",gpsstunde,gpsminute,gpssekunde,gpsTag,gpsMonat,gpsJahr);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,5*charhighte);}
		
		
		//Druckdaten	
		buffersize=sprintf(buffer,"%.02f",druck.pressure);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,6*charhighte);}
		
		buffersize=sprintf(buffer,"%.02f",druck.altitude);
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,7*charhighte);}
		
			
		send();	
	}
	
};

class einstellungen: public monitor
{
	private:
	public:
	einstellungen(Display *ol,RTC *rt): monitor(ol,rt)
	{
		char na[] = "Settings";
		for(uint8_t i =0; i< namesize;i++){
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
		}
		maxentries = 5;
	}
	
	//Tastenhandler
	uint8_t tastendruck(uint8_t *tast){
		return 0;
	}
	
	//anzeige erstellen
	void draw(){
		monitor::draw();
		header();
		bottom();
		if (posx==0)
		{
			buffersize=sprintf(buffer,"Zeit einstellen");
			for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,2*charhighte);}
			buffersize=sprintf(buffer,"g bestimmen");
			for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,3*charhighte);}
			buffersize=sprintf(buffer,"Versionsnummer");
			for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,4*charhighte);}
			buffersize=sprintf(buffer,"Mount SD: %u", (bool)(statusreg&(1<<mounttingstat)));
			for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,5*charhighte);}
			buffersize=sprintf(buffer,"Logging:  %u", (bool)(statusreg&(1<<loggingstat)));
			for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,6*charhighte);}
			oled->draw_ASCI('>',0*charsize,(posy+2)*charhighte);
		}
		else if (posy==0 && posx==1)
		{
			//Uhreinstellung machen
			//noch etwas bloed, aber mit eigener Funktion
			uhreinstellen();
			posx=0;
		}
		else if (posy==1 && posx==1)
		{
			//g bestimmen einbauen
			buffersize=sprintf(buffer,"Geraet nicht bewegen");
			for (uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
			buffersize=sprintf(buffer,"* fuer start");
			for (uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,3*charhighte);}
			if (5<6&&5>0)
			{
				buffersize=sprintf(buffer,"Noch %i Sekunden",5);
				for (uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,5*charhighte);}
			}
			else if (5==0)
			{
				posx--;
			}
		}
		else if (posy==2 && posx==1)
		{
			buffersize=sprintf(buffer,"Version: %.2f",(double)VERSIONSNUMMER);
			for(uint8_t i=0; i < buffersize;i++){
				oled->draw_ASCI(buffer[i],(i+2)*charsize,3.5*charhighte);

			}
		}
		send();	
	}
	
};

class wilkommen: public monitor
{
	private:
	
	public:
	wilkommen(Display *ol, RTC *rt):monitor(ol,rt)
	{

	}
	
	void draw(){
		oled->clearFrame();
		
		buffersize=sprintf(buffer,"Fahradcomputer");
		for(uint8_t i=0; i < buffersize;i++){
			oled->draw_ASCI(buffer[i],i*charsize,0);
		}
		buffersize=sprintf(buffer,"--------------");
		for(uint8_t i=0; i < buffersize;i++){
			oled->draw_ASCI(buffer[i],i*charsize,8);
		}
		buffersize=sprintf(buffer,"Version: %.2f",(double)VERSIONSNUMMER);
		for(uint8_t i=0; i < buffersize;i++){
			oled->draw_ASCI(buffer[i],i*charsize,32);

		}
		send();
	}
		
};

class menue: public monitor
{
	private:
	public:
	menue(Display *ol, RTC *rt):monitor(ol,rt)
	{
		char na[] = "Menue";
		for(uint8_t i =0; i< namesize;i++)
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
			maxentriesx = 0;
			maxentries = numberofpages;
	}
	
	uint8_t tastendruck(uint8_t *tast){
		return 0;
	}

	void draw(){
		monitor::draw();
		header();
		bottom();
		//menueeintrag zeichnen
		buffersize=sprintf(buffer,"Uhr");
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,2*charhighte);}
		buffersize=sprintf(buffer,"Tacho");
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,3*charhighte);}
		buffersize=sprintf(buffer,"Wandern");
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,4*charhighte);}
		buffersize=sprintf(buffer,"Einstellungen");
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,5*charhighte);}
		buffersize=sprintf(buffer,"Display aus");
		for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,6*charhighte);}
		oled->draw_ASCI('>',0*charsize,(posy+2)*charhighte);
		send();
	}
	
};

class offscreen: public monitor
{
	private:
	
	public:
	offscreen(Display *ol, RTC *rt):monitor(ol,rt){
		
	}
	
	void draw(){
		monitor::draw();
		send();
	}
};

#endif /* MONITOR_H_ */