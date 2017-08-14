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
	virtual void header(){
		for(uint8_t i=0;i<8;i++){
			oled->draw_ASCI(rtc->msg_uhr[i],i*charsize,0);
		}
		oled->drawBATT(Batteriestatus(),SSD1306_WIDTH-13-4,0);
		oled->drawGPS((gpsstatus&((1<<enable)|(1<<fix)))>>enable,SSD1306_WIDTH-27,0);
		/*
		buffersize=sprintf(buffer,"Bat: %i%%",Batteriestatus());
		for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
			oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
		}*/
	}
	
	//draw bottom and overload it
	virtual void bottom(){
		for(uint8_t i=0;i<8;i++){
			oled->draw_ASCI(rtc->msg_dat[i],i*charsize+(SSD1306_WIDTH-bitsderrtc*charsize),7*charhighte);
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
			if (i<sizeof(na)){name[i] = na[i];}
			else{name[i] = ' ';}
		}
		Zeiger = 0;
		//maxentries = 3;
        maxentriesx = 3;
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
			if (Zeiger==1){setzeiger(0);}
			else{setzeiger(1);}
		}
        if(posx == 1){
            if(*tast == 'A'){
                if(rtc->WStunden<23){rtc->WStunden += 1;}
                else{rtc->WStunden = 0;}
           } 
           else if(*tast == 'B'){
                if(rtc->WStunden>0){rtc->WStunden -= 1;}
                else{rtc->WStunden = 23;}
           }
        }
        else if(posx == 2){
            if(*tast == 'A'){
                if(rtc->WMinuten<59){rtc->WMinuten += 1;}
                else{rtc->WMinuten = 0;}
           } 
           else if(*tast == 'B'){
                if(rtc->WMinuten>0){rtc->WMinuten -= 1;}
                else{rtc->WMinuten = 59;}
           }
        }
	return 0;
	}

	//header
	void header(){
		for(uint8_t i=0;i<(bitsderrtc-3);i++){oled->draw_ASCI(rtc->msg_uhr[i],65+i*charsize,0*charhighte);}
		oled->drawBATT(Batteriestatus(),SSD1306_WIDTH-13-4,0);
		oled->drawGPS((gpsstatus&((1<<enable)|(1<<fix)))>>enable,SSD1306_WIDTH-27,0);
		/*
		buffersize=sprintf(buffer,"%i%%",Batteriestatus());
		for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
			oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
		}*/
	}
	void bottom(){
		for(uint8_t i=0;i<bitsderrtc;i++){oled->draw_ASCI(rtc->msg_dat[i],65+i*charsize,7*charhighte);}
	}

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header();
		bottom();
        if(posx==3){
           EEPWriteByte(EEWECKSTUNDEN,rtc->WStunden); 
           EEPWriteByte(EEWECKSTUNDEN,rtc->WStunden); 
           posx = 0;
        }
        if(posx==0){
            oled->analog(rtc->Stunden,rtc->Minuten,rtc->Sekunden,Zeiger);
            //draw large number
            oled->draw_number16x16(rtc->msg_uhr[0]-'0',70,1.66*charhighte);
            oled->draw_number16x16(rtc->msg_uhr[1]-'0',70+numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->msg_uhr[3]-'0',70,2.33*charhighte+numbersmalhight);
            oled->draw_number16x16(rtc->msg_uhr[4]-'0',70+numbersmalsize,2.33*charhighte+numbersmalhight);
        }
        else if(posx==1){
            oled->draw_number16x16(rtc->WStunden/10,0*numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->WStunden%10,1*numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->WMinuten/10,3*numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->WMinuten%10,4*numbersmalsize,1.66*charhighte);
            oled->draw_ASCI('^',1*numbersmalsize+charsize,4*charhighte);
            //oled->draw_ASCI('-',70-3*charsize,2*charhighte);
            //oled->draw_ASCI('-',70-2*charsize,2*charhighte);
        }
        else if(posx==2){
            oled->draw_number16x16(rtc->WStunden/10,0*numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->WStunden%10,1*numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->WMinuten/10,3*numbersmalsize,1.66*charhighte);
            oled->draw_number16x16(rtc->WMinuten%10,4*numbersmalsize,1.66*charhighte);
            oled->draw_ASCI('^',4*numbersmalsize+charsize,4*charhighte);
            //oled->draw_number16x16(rtc->WStunden/10,70,1.66*charhighte);
            //oled->draw_number16x16(rtc->WStunden%10,70+numbersmalsize,1.66*charhighte);
            //oled->draw_number16x16(rtc->WMinuten/10,70,2.33*charhighte+numbersmalhight);
            //oled->draw_number16x16(rtc->WMinuten%10,70+numbersmalsize,2.33*charhighte+numbersmalhight);
            //oled->draw_ASCI('-',70-3*charsize,3*charhighte+numbersmalhight);
            //oled->draw_ASCI('-',70-2*charsize,3*charhighte+numbersmalhight);
            //oled->draw_ASCI('>',70-1*charsize,3*charhighte+numbersmalhight);
        }
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
            accel.reset_Magn();
		}
		return 0;
	}

	//header 
	void header(){
		oled->drawBATT(Batteriestatus(),SSD1306_WIDTH-13-4,0);
		oled->drawGPS((gpsstatus&((1<<enable)|(1<<fix)))>>enable,SSD1306_WIDTH-27,0);
		/*
		buffersize=sprintf(buffer,"Bat.: %i%%",Batteriestatus());
		for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
			oled->draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
		}*/
	}

	//Anzeige schalten
	void draw(){
		monitor::draw();
		header();
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
        accel.magn_read_angle();
        oled->draw_line(31,31,15,(accel.angle_M+M_PI_2)+M_PI);
        oled->drawRectangle(29,29,33,33,1);

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
        maxentriesx = 7;
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
            /*
			EEPROM_Write(EEMINUTEN,rtc->Minuten);
			EEPROM_Write(EESTUNDEN,rtc->Stunden);
			EEPROM_Write(EETAGE,rtc->Tag);
			EEPROM_Write(EEMONAT,rtc->Monat);
			EEPROM_Write(EEJAHR,rtc->Jahr);*/
			EEPWriteByte(EEMINUTEN,rtc->Minuten);
			EEPWriteByte(EESTUNDEN,rtc->Stunden);
			EEPWriteByte(EETAGE,rtc->Tag);
			EEPWriteByte(EEMONAT,rtc->Monat);
			EEPWriteByte(EEJAHR,rtc->Jahr);
		}
		else if (*tast=='A')
		{
             int8_t dlatgra = (int8_t) dlat;
            uint8_t dlatmin = (uint8_t) ((uint32_t)(dlat*60.)%60);
            uint8_t dlatsek = (uint8_t) ((uint32_t)(dlat*3600.)%60);
             int8_t dlongra = (int16_t) dlon;
            uint8_t dlonmin = (uint8_t) ((uint32_t)(dlon*60.)%60);
            uint8_t dlonsek = (uint8_t) ((uint16_t)(dlon*3600.)%60);
            switch (posx)
            {
                case 0:
                    if(gpsstatus&(1<<enable)){GPSDISABLE();}
                    else{GPSENABLE();}
                    break;
                case 1:
                    if(dlatgra < 89.){  dlat += 1.;}
                    break;
                case 2:
                    if(dlatmin < 59){   dlat += 1./60.;}
                    break;
                case 3:
                    if(dlatsek < 59){   dlat += 1./3600.;}
                    break;
                case 4:
                    if(dlongra < 179.){ dlon += 1.;}
                    break;
                case 5:
                    if(dlonmin < 59){   dlon += 1./60.;}
                    break;
                case 6:
                    if(dlonsek < 59){   dlon += 1./3600.;}
                    break;
            }
		}
		else if (*tast=='B')
		{
             int8_t dlatgra = (int8_t) dlat;
            uint8_t dlatmin = (uint8_t) ((uint32_t)(dlat*60.)%60);
            uint8_t dlatsek = (uint8_t) ((uint32_t)(dlat*3600.)%60);
             int8_t dlongra = (int16_t) dlon;
            uint8_t dlonmin = (uint8_t) ((uint32_t)(dlon*60.)%60);
            uint8_t dlonsek = (uint8_t) ((uint16_t)(dlon*3600.)%60);
            switch (posx)
            {
                case 0:
                    dlat = lat;
                    dlon = lon;
                    break;
                case 1:
                    if(dlatgra > -89.){  dlat -= 1.;}
                    break;
                case 2:
                    if(dlatmin > 0 ){    dlat -= 1./60.;}
                    break;
                case 3:
                    if(dlatsek > 0 ){    dlat -= 1./3600.;}
                    break;
                case 4:
                    if(dlongra > -179.){ dlon -= 1.;}
                    break;
                case 5:
                    if(dlonmin > 0 ){    dlon -= 1./60.;}
                    break;
                case 6:
                    if(dlonsek > 0 ){    dlon -= 1./3600.;}
                    break;
            }
		}
		else if (*tast=='X')
		{
			druck.pressure0 = druck.pressure;
		}
		else if (*tast=='Y')
		{
            accel.reset_Magn();
		}
		return 0;
	}
	
	void draw(){
		monitor::draw();
		header();
		bottom();
		
		//Umrechnen in Grad Minuten und Sekunden
         int8_t latgra = (int8_t) lat;
        uint8_t latmin = (uint8_t) ((uint32_t)(lat*60.)%60);
        uint8_t latsek = (uint8_t) ((uint32_t)(lat*3600.)%60);
         int8_t longra = (int16_t) lon;
        uint8_t lonmin = (uint8_t) ((uint32_t)(lon*60.)%60);
        uint8_t lonsek = (uint8_t) ((uint16_t)(lon*3600.)%60);

         int8_t dlatgra = (int8_t) dlat;
        uint8_t dlatmin = (uint8_t) ((uint32_t)(dlat*60.)%60);
        uint8_t dlatsek = (uint8_t) ((uint32_t)(dlat*3600.)%60);
         int8_t dlongra = (int16_t) dlon;
        uint8_t dlonmin = (uint8_t) ((uint32_t)(dlon*60.)%60);
        uint8_t dlonsek = (uint8_t) ((uint16_t)(dlon*3600.)%60);
        float dist     = get_distance(lat,lon,dlat,dlon);
        float bear     = get_bearing(lat,lon,dlat,dlon);

        if(posx == 0){
            //Latitude
            buffersize=sprintf(buffer,"Lat: %03i %02i'%02i''",latgra,latmin,latsek);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,1*charhighte);}
            //Longitude
            buffersize=sprintf(buffer,"Lon: %03i %02i'%02i''",longra,lonmin,lonsek);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}

            if(dist > 1e3){
                buffersize=sprintf(buffer,"Dist: %.1fkm",dist/1000.);
            }
            else{
                buffersize=sprintf(buffer,"Dist: %.1fm",dist);
            }
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,3*charhighte);}
            
                
            //Speed
            buffersize=sprintf(buffer,"Spe: %.2f",gpsspeed);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
                
            //gpstime
            buffersize=sprintf(buffer,"%02i:%02i:%02i %02i.%02i.%02i",gpsstunde,gpsminute,gpssekunde,gpsTag,gpsMonat,gpsJahr);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,5*charhighte);}
            
            
            //Druckdaten	
            buffersize=sprintf(buffer,"%.02f  %.02f",druck.pressure,druck.temperature);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,6*charhighte);}
            
            buffersize=sprintf(buffer,"%.02f",druck.altitude);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,7*charhighte);}
        }
        else if(posx==1){
            //Latitude
            buffersize=sprintf(buffer,"Set Destination");
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
            buffersize=sprintf(buffer,"Lat Gra: %03i ",dlatgra);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
        }
        else if(posx==2){
            //Latitude
            buffersize=sprintf(buffer,"Set Destination");
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
            buffersize=sprintf(buffer,"Lat Min: %03i'",dlatmin);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
        }
        else if(posx==3){
            //Latitude
            buffersize=sprintf(buffer,"Set Destination");
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
            buffersize=sprintf(buffer,"Lat Sek: %03i''",dlatsek);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
        }
        else if(posx==4){
            //Longitude
            buffersize=sprintf(buffer,"Set Destination");
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
            buffersize=sprintf(buffer,"Lon Gra: %03i",dlongra);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
        }
        else if(posx==5){
            //Longitude
            buffersize=sprintf(buffer,"Set Destination");
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
            buffersize=sprintf(buffer,"Lon Min: %03i'",dlonmin);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
        }
        else if(posx==6){
            //Longitude
            buffersize=sprintf(buffer,"Set Destination");
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,2*charhighte);}
            buffersize=sprintf(buffer,"Lon Sek: %03i''",dlonsek);
            for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize,4*charhighte);}
        }
        else if(posx==7){
            if(dist>=1e6){
                oled->draw_number16x16((uint8_t)(dist/1e7),0,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e6)%10,numbersmalsize,1.66*charhighte);
                oled->draw_ASCI('M',2*numbersmalsize+charsize/2,2*charhighte);
                oled->draw_ASCI('m',2*numbersmalsize+charsize+charsize/2,2*charhighte);
            }
            else if(dist>=1e5){
                oled->draw_number16x16((uint8_t)(dist/1e5)%10,0,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e4)%10,numbersmalsize,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e3)%10,2*numbersmalsize,1.66*charhighte);
                oled->draw_ASCI('k',3*numbersmalsize+charsize/2,2*charhighte);
                oled->draw_ASCI('m',3*numbersmalsize+charsize+charsize/2,2*charhighte);
            }
            else if(dist>=1e4){
                oled->draw_number16x16((uint8_t)(dist/1e4)%10,0,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e3)%10,numbersmalsize,1.66*charhighte);
                oled->draw_ASCI('.',charsize/2+2*numbersmalsize,2.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e3)%10,2*numbersmalsize+charsize,1.66*charhighte);
                oled->draw_ASCI('k',3*numbersmalsize+charsize/2+charsize,2*charhighte);
                oled->draw_ASCI('m',3*numbersmalsize+charsize+charsize/2+charsize,2*charhighte);
            }
            else if(dist>=1e3){
                oled->draw_number16x16((uint8_t)(dist/1e3)%10,0,1.66*charhighte);
                oled->draw_ASCI('.',charsize/2+1*numbersmalsize,2.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e2)%10,numbersmalsize+charsize,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e1)%10,2*numbersmalsize+charsize,1.66*charhighte);
                oled->draw_ASCI('k',3*numbersmalsize+charsize/2+charsize,2*charhighte);
                oled->draw_ASCI('m',3*numbersmalsize+charsize+charsize/2+charsize,2*charhighte);
            }
            else if(dist>=1e2){
                oled->draw_number16x16((uint8_t)(dist/1e2)%10,0,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e1)%10,numbersmalsize,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist)%10,2*numbersmalsize,1.66*charhighte);
                oled->draw_ASCI('m',3*numbersmalsize+charsize/2,2*charhighte);
            }
            else if(dist>=1e1){
                oled->draw_number16x16((uint8_t)(dist/1e1)%10,0,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist/1e0)%10,numbersmalsize,1.66*charhighte);
                oled->draw_ASCI('.',charsize/2+2*numbersmalsize,2.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist*10)%10,2*numbersmalsize+charsize,1.66*charhighte);
                oled->draw_ASCI('m',3*numbersmalsize+charsize/2+charsize,2*charhighte);
            }
            else{
                oled->draw_number16x16((uint8_t)(dist)%10,0,1.66*charhighte);
                oled->draw_ASCI('.',charsize/2+1*numbersmalsize,2.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist*10)%10,numbersmalsize+charsize,1.66*charhighte);
                oled->draw_number16x16((uint8_t)(dist*100)%10,2*numbersmalsize+charsize,1.66*charhighte);
                oled->draw_ASCI('m',3*numbersmalsize+charsize/2+charsize,2*charhighte);
            }
            accel.magn_read_angle();
            //oled->draw_number16x16((uint8_t)(bear*180./M_PI/100)%10,0*numbersmalsize,1.66*charhighte+numbersmalhight);
            //oled->draw_number16x16((uint8_t)(bear*180./M_PI/10)%10,1*numbersmalsize,1.66*charhighte+numbersmalhight);
            //oled->draw_number16x16((uint8_t)(bear*180./M_PI)%10,2*numbersmalsize,1.66*charhighte+numbersmalhight);
            oled->draw_number16x16((uint8_t)(gpsspeed/100)%10,0*numbersmalsize,1.66*charhighte+numbersmalhight);
            oled->draw_number16x16((uint8_t)(gpsspeed/10)%10,1*numbersmalsize,1.66*charhighte+numbersmalhight);
            oled->draw_number16x16((uint8_t)(gpsspeed)%10,2*numbersmalsize,1.66*charhighte+numbersmalhight);

            oled->draw_number16x16((uint8_t)(accel.angle_M*180./M_PI/100)%10,0*numbersmalsize,1.66*charhighte+numbersmalhight*2);
            oled->draw_number16x16((uint8_t)(accel.angle_M*180./M_PI/10)%10,1*numbersmalsize,1.66*charhighte+numbersmalhight*2);
            oled->draw_number16x16((uint8_t)(accel.angle_M*180./M_PI)%10,2*numbersmalsize,1.66*charhighte+numbersmalhight*2);
            float winkel = accel.angle_M-bear;
            oled->draw_line(100,31,15,winkel+M_PI_2+M_PI);
            oled->drawRectangle(98,29,102,33,1);
        }
			
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
			//buffersize=sprintf(buffer,"Zeit einstellen");
			buffersize=sprintf(buffer,"Wecker Ein: %u", (bool)(rtc->interupts&(1<<Weckerein)));
			for(uint8_t i=0;i<buffersize;i++){oled->draw_ASCI(buffer[i],i*charsize+2*charsize,2*charhighte);}
			//buffersize=sprintf(buffer,"Wecker einstellen");
			buffersize=sprintf(buffer,"Wecker Dauer:  %u", (bool)(rtc->interupts&(1<<Weckerdauer)));
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
			//uhreinstellen();
            if(rtc->interupts&(1<<Weckerein)){rtc->interupts&=~(1<<Weckerein);}
            else{rtc->interupts|=(1<<Weckerein);}
			posx=0;
		}
		else if (posy==1 && posx==1)
		{
            if(rtc->interupts&(1<<Weckerdauer)){rtc->interupts&=~(1<<Weckerdauer);}
            else{rtc->interupts|=(1<<Weckerdauer);}
			posx=0;
		}
		else if (posy==2 && posx==1)
		{
			buffersize=sprintf(buffer,"Version: %.2f",VERSIONSNUMMER);
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
		buffersize=sprintf(buffer,"Version: %.2f",(float)VERSIONSNUMMER);
		for(uint8_t i=0; i < buffersize;i++){
			oled->draw_ASCI(buffer[i],i*charsize,32);

		}
		send();
	}
		
};

const char entries_men[][14] PROGMEM = {
    "Uhr          ",
    "Tacho        ",
    "GPS          ",
    "GPS read     ",
    "GPS save     ",
    "Einstellungen",
    "Display aus  "};
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
        int8_t places[5] = {posy-2,
                            posy-1,
                            posy,
                            posy+1,
                            posy+2};
        for(uint8_t i=0; i<5; i++){
            if(places[i]>=maxentries){
                places[i] -= maxentries;
            }
            else if(places[i]<0){
                places[i] += maxentries;
            }
        }
		for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[0]][i]),i*charsize+2*charsize,2*charhighte);}
		for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[1]][i]),i*charsize+2*charsize,3*charhighte);}
		for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[2]][i]),i*charsize+2*charsize,4*charhighte);}
		for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[3]][i]),i*charsize+2*charsize,5*charhighte);}
		for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[4]][i]),i*charsize+2*charsize,6*charhighte);}
		oled->draw_ASCI('>',0*charsize,(2+2)*charhighte);
		send();
	}
	
};

const char entries_save[][14] PROGMEM = {
    "Letztes1     ",
    "Letztes2     ",
    "Parkplatz    "};
class GPS_save: public monitor
{
	private:
	public:
	GPS_save(Display *ol, RTC *rt):monitor(ol,rt)
	{
		char na[] = "GPS save";
		for(uint8_t i =0; i< namesize;i++)
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
		maxentriesx = 1;
		maxentries = 3;
	}
	
	uint8_t tastendruck(uint8_t *tast){
		return 0;
	}

	void draw(){
		monitor::draw();
		header();
		bottom();
        if(posx==1 && posy == 0){
            EEPROM_write_float(EEGPSLAST1  ,dlat);
            EEPROM_write_float(EEGPSLAST1+4,dlon);
            posx = 0;
        }
        if(posx==1 && posy == 1){
            EEPROM_write_float(EEGPSLAST2  ,dlat);
            EEPROM_write_float(EEGPSLAST2+4,dlon);
            posx = 0;
        }
        if(posx==1 && posy == 2){
            EEPROM_write_float(EEGPSPARK  ,dlat);
            EEPROM_write_float(EEGPSPARK+4,dlon);
            posx = 0;
        }
        if(posx==0){
            int8_t places[3] = {posy-1,
                                posy,
                                posy+1};
            for(uint8_t i=0; i<3; i++){
                if(places[i]>=maxentries){
                    places[i] -= maxentries;
                }
                else if(places[i]<0){
                    places[i] += maxentries;
                }
            }
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_save[(uint8_t)places[0]][i]),i*charsize+2*charsize,2*charhighte);}
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_save[(uint8_t)places[1]][i]),i*charsize+2*charsize,3*charhighte);}
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_save[(uint8_t)places[2]][i]),i*charsize+2*charsize,4*charhighte);}
            //for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[3]][i]),i*charsize+2*charsize,5*charhighte);}
            //for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_men[(uint8_t)places[4]][i]),i*charsize+2*charsize,6*charhighte);}
            oled->draw_ASCI('>',0*charsize,(1+2)*charhighte);
        }
		send();
	}
	
};

const char entries_read[][14] PROGMEM = {
    "Stemmen      ",
    "Goettingen   ",
    "Letzte1      ",
    "Letzte2      ",
    "Parkplatz    "};
class GPS_read: public monitor
{
	private:
	public:
	GPS_read(Display *ol, RTC *rt):monitor(ol,rt)
	{
		char na[] = "GPS read";
		for(uint8_t i =0; i< namesize;i++)
			if (i<sizeof(na))
			{
				name[i] = na[i];
			}
			else
			{
				name[i] = ' ';
			}
		maxentriesx = 1;
		maxentries = 5;
	}
	
	uint8_t tastendruck(uint8_t *tast){
		return 0;
	}

	void draw(){
		monitor::draw();
		header();
		bottom();
        if(posx==1 && posy == 0){
            dlat = EEPROM_read_float(EEGPSHOME  );
            dlon = EEPROM_read_float(EEGPSHOME+4);
            posx = 0;
        }
        if(posx==1 && posy == 1){
            dlat = EEPROM_read_float(EEGPSGOET  );
            dlon = EEPROM_read_float(EEGPSGOET+4);
            posx = 0;
        }
        if(posx==1 && posy == 2){
            dlat = EEPROM_read_float(EEGPSLAST1  );
            dlon = EEPROM_read_float(EEGPSLAST1+4);
            posx = 0;
        }
        if(posx==1 && posy == 3){
            dlat = EEPROM_read_float(EEGPSLAST2  );
            dlon = EEPROM_read_float(EEGPSLAST2+4);
            posx = 0;
        }
        if(posx==1 && posy == 4){
            dlat = EEPROM_read_float(EEGPSPARK  );
            dlon = EEPROM_read_float(EEGPSPARK+4);
            posx = 0;
        }
        if(posx==0){
            int8_t places[5] = {posy-2,
                                posy-1,
                                posy,
                                posy+1,
                                posy+2};
            for(uint8_t i=0; i<5; i++){
                if(places[i]>=maxentries){
                    places[i] -= maxentries;
                }
                else if(places[i]<0){
                    places[i] += maxentries;
                }
            }
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_read[(uint8_t)places[0]][i]),i*charsize+2*charsize,2*charhighte);}
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_read[(uint8_t)places[1]][i]),i*charsize+2*charsize,3*charhighte);}
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_read[(uint8_t)places[2]][i]),i*charsize+2*charsize,4*charhighte);}
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_read[(uint8_t)places[3]][i]),i*charsize+2*charsize,5*charhighte);}
            for(uint8_t i=0;i<14;i++){oled->draw_ASCI(pgm_read_byte(&entries_read[(uint8_t)places[4]][i]),i*charsize+2*charsize,6*charhighte);}
            oled->draw_ASCI('>',0*charsize,(2+2)*charhighte);
        }
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
