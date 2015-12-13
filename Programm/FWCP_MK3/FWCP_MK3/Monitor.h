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

class monitor
{
	private:
	
	protected:
		char buffer[20];
		uint8_t buffersize;
		Display *oled;
		RTC *rtc;
	public:
	monitor(Display *ol, RTC *rt)
	{
		buffersize = 0;
		oled = ol;
		rtc = rt;
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
	public:
	uhr(Display *ol, RTC *rt):monitor(ol,rt)
	{
	}
	
	//Taster ueberpruefen
	

	//anzeige vorbereiten
	void draw()
	{
		monitor::draw();
		header(1);
		bottom(2);
		oled->analog(rtc->Stunden,rtc->Minuten,rtc->Sekunden,1);
		//draw large number
		oled->draw_number16x16(rtc->msg_uhr[0]-'0',70,1.66*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[1]-'0',70+numbersmalsize,1.66*charhighte);
		oled->draw_number16x16(rtc->msg_uhr[3]-'0',70,2.33*charhighte+numbersmalhight);
		oled->draw_number16x16(rtc->msg_uhr[4]-'0',70+numbersmalsize,2.33*charhighte+numbersmalhight);
		monitor::send();
	}
};

#endif /* MONITOR_H_ */