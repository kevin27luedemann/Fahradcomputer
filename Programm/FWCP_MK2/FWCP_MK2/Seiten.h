/*
 * Seiten.h
 *
 * Created: 27.08.2015 10:51:53
 *  Author: kevin
 */ 


#ifndef SEITEN_H_
#define SEITEN_H_

//Diese Funktionen schreiben nur in den Framebuffer und das Schreiben passiert in der Main ueber einen Interupt

void willkommenpage(){
	char buffer[20];
	uint8_t buffersize;
	oled.clearFrame();
	buffersize=sprintf(buffer,"Fahradcomputer");
	for(uint8_t i=0; i < buffersize;i++){
		oled.draw_ASCI(buffer[i],i*charsize,0);
	}
	buffersize=sprintf(buffer,"--------------");
	for(uint8_t i=0; i < buffersize;i++){
		oled.draw_ASCI(buffer[i],i*charsize,8);
	}
	buffersize=sprintf(buffer,"Version: %.2f",(double)VERSIONSNUMMER);
	for(uint8_t i=0; i < buffersize;i++){
		oled.draw_ASCI(buffer[i],i*charsize,32);

	}
	oled.sendFrame();
	oled.clearFrame();
	for (uint8_t i=0;i<5;i++)
	{
		_delay_ms(50);
		_delay_ms(50);
	}
}

void header(uint8_t page){
	char buffer[20];
	uint8_t buffersize=0;;
	switch (page) {
		case 0:	//standard Header fuer fast alle Pages
			for(uint8_t i=0;i<8;i++){
				oled.draw_ASCI(rtc.msg_uhr[i],i*charsize,0);
			}
			buffersize=sprintf(buffer,"Bat: %i%%",Batteriestatus());
			for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
				oled.draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
			}
			break;
		case 1:
			buffersize=sprintf(buffer,"Bat: %i%%",Batteriestatus());
			for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
				oled.draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
			}
			break;
		case 2:
			buffersize=sprintf(buffer,"%i%%",Batteriestatus());
			for(uint8_t i=((SSD1306_WIDTH/charsize)-buffersize);i<(SSD1306_WIDTH/charsize);i++){
				oled.draw_ASCI(buffer[i-((SSD1306_WIDTH/charsize)-buffersize)],i*charsize,0);
			}
			break;
		default:
			break;
	}
}
void bottom(uint8_t page){
	switch (page) {
		case 0:	//standard Header fuer fast alle Pages
			for(uint8_t i=0;i<8;i++){
				oled.draw_ASCI(rtc.msg_dat[i],i*charsize+(SSD1306_WIDTH-bitsderrtc*charsize),7*charhighte);
			}
			break;
		case 1:
			for(uint8_t i=0;i<8;i++){
				oled.draw_ASCI(rtc.msg_dat[i],5+i*charsize,7*charhighte);
			}
			break;
		default:
			break;
		}
}

void anzeige_richtung(float winkel, uint8_t x, uint8_t y){
	//finden der Ziffer fuer die Anzeige
	if ((winkel<45/2) || (winkel >= 360-45/2))
	{
		oled.draw_ASCI('N',x,y*charhighte);
	}
	else if (winkel<90-45/2)
	{
		oled.draw_ASCI('N',x-charsize/2,y*charhighte);
		oled.draw_ASCI('O',x+charsize/2,y*charhighte);
	}
	else if (winkel<135-45/2)
	{
		oled.draw_ASCI('O',x,y*charhighte);
	}
	else if (winkel<180-45/2)
	{
		oled.draw_ASCI('S',x-charsize/2,y*charhighte);
		oled.draw_ASCI('O',x+charsize/2,y*charhighte);
	}
	else if (winkel<225-45/2)
	{
		oled.draw_ASCI('S',x,y*charhighte);
	}
	else if (winkel<270-45/2)
	{
		oled.draw_ASCI('S',x-charsize/2,y*charhighte);
		oled.draw_ASCI('W',x+charsize/2,y*charhighte);
	}
	else if (winkel<315-45/2)
	{
		oled.draw_ASCI('W',x,y*charhighte);
	}
	else if (winkel<360-45/2)
	{
		oled.draw_ASCI('N',x-charsize/2,y*charhighte);
		oled.draw_ASCI('W',x+charsize/2,y*charhighte);
	}
}
void anzeige_nadel(uint8_t x, uint8_t y, float angle){
	//fixed parameters for drawing the arrow
	oled.draw_triangle(x,y,20,70,angle);
}
void anzeige_kleinenadel(uint8_t x, uint8_t y, float angle){
	//fixed parameters for drawing the arrow
	oled.draw_triangle(x,y,10,70,angle);
}

void menue(uint8_t pos){
	
	oled.clearFrame();
	header(0);
	bottom(0);

}

void GOL_seite(){
	for (uint16_t i=0; i<GOL_SIZE;i++)
	{
		oled.Displayframe[i]=gol.playground[i];
	}
}

void menue_haupt(){
	char buffer[20];
	uint8_t buffersize=0;

	oled.clearFrame();
	buffersize=sprintf(buffer,"1:Fahradanzeige");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	buffersize=sprintf(buffer,"2:Taschenrechner");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,1*charhighte);}
	buffersize=sprintf(buffer,"3:Kompass");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"4:Uhrmenue");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	buffersize=sprintf(buffer,"5:Einstellungen");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	buffersize=sprintf(buffer,"6:Display aus");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	buffersize=sprintf(buffer,"7:Drucksensor");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,6*charhighte);}
}
void menue_einst(){
	char buffer[20];
	uint8_t buffersize=0;

	oled.clearFrame();
	buffersize=sprintf(buffer,"1:Kompass kalib.");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	buffersize=sprintf(buffer,"2:Kompassverstaerkung");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,1*charhighte);}
	buffersize=sprintf(buffer,"3:Uhr einstellen");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"4:Weckereinstellen");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	buffersize=sprintf(buffer,"5:g messen");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	//buffersize=sprintf(buffer,"6:HMC5883L Gain");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	//buffersize=sprintf(buffer,"7:Uhr einstellen");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,6*charhighte);}
}
void menue_uhr(){
	char buffer[20];
	uint8_t buffersize=0;

	oled.clearFrame();
	buffersize=sprintf(buffer,"1:Uhr");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	buffersize=sprintf(buffer,"2:Stoppuhr");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,1*charhighte);}
	buffersize=sprintf(buffer,"3:Timer");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"4:Wecker");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	buffersize=sprintf(buffer,"5:Uhr gross");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	//buffersize=sprintf(buffer,"6:Wecker");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	//buffersize=sprintf(buffer,"7:Uhr einstellen");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,6*charhighte);}
}

void find_gravity(){
	uint8_t buffersize;
	char buffer[20];
	oled.clearFrame();
	header(0);
	bottom(0);
	buffersize=sprintf(buffer,"Geraet nicht bewegen");
	for (uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"* fuer start");
	for (uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	if (pos<6)
	{
		buffersize=sprintf(buffer,"Noch %i Sekunden",pos);
		for (uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	}
}

void uhranzeigen(){
	uint8_t buffersize;
	char Buffer[20];
	oled.clearFrame();
	oled.analog(rtc.Stunden,rtc.Minuten,rtc.Sekunden,1);
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_uhr[i],65+i*charsize,0*charhighte);}
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_dat[i],65+i*charsize,7*charhighte);}
	buffersize=sprintf(Buffer,"%i%%",Batteriestatus());
	for(uint8_t i=0;i<buffersize;i++){
		if (Buffer[i]=='%' || Buffer[i]=='-')
		{
			oled.draw_ASCI(Buffer[i],73+i*numbersmalsize,3*charhighte);
		}
		else{
			oled.draw_number16x16(Buffer[i]-'0',70+i*numbersmalsize,2.5*charhighte);
		}
	}
}

void uhranzeigenmin(){
	oled.clearFrame();
	header(2);
	oled.analog(rtc.Stunden,rtc.Minuten,rtc.Sekunden,0);
	for(uint8_t i=0;i<(bitsderrtc-3);i++){oled.draw_ASCI(rtc.msg_uhr[i],65+i*charsize,0*charhighte);}
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_dat[i],65+i*charsize,7*charhighte);}
	//show hours and minutes in big numbers
	oled.draw_number16x16(rtc.msg_uhr[0]-'0',70,1.66*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[1]-'0',70+numbersmalsize,1.66*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[3]-'0',70,2.33*charhighte+numbersmalhight);
	oled.draw_number16x16(rtc.msg_uhr[4]-'0',70+numbersmalsize,2.33*charhighte+numbersmalhight);
	//buffersize=sprintf(Buffer,"%i%%",Batteriestatus());
	//for(uint8_t i=0;i<buffersize;i++){
		//if (Buffer[i]=='%' || Buffer[i]=='-')
		//{
			//oled.draw_ASCI(Buffer[i],73+i*numbersmalsize,3.5*charhighte);
		//}
		//else{
			//oled.draw_number16x16(Buffer[i]-'0',70+i*numbersmalsize,3*charhighte);
		//}
	//}
}

void Weckeranzeige(){
	oled.clearFrame();
	bottom(1);
	header(1);
	//Uhr
	oled.draw_number16x16(rtc.msg_uhr[0]-'0',5,1.66*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[1]-'0',5+numbersmalsize,1.66*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[3]-'0',5,2.33*charhighte+numbersmalhight);
	oled.draw_number16x16(rtc.msg_uhr[4]-'0',5+numbersmalsize,2.33*charhighte+numbersmalhight);
	
	//Weckerzeit
	oled.draw_number16x16(rtc.WStunden/10,70,1.66*charhighte);
	oled.draw_number16x16(rtc.WStunden%10,70+numbersmalsize,1.66*charhighte);
	oled.draw_number16x16(rtc.WMinuten/10,70,2.33*charhighte+numbersmalhight);
	oled.draw_number16x16(rtc.WMinuten%10,70+numbersmalsize,2.33*charhighte+numbersmalhight);
	
	//Weckeraktiv Zeichen
	if ((rtc.interupts&(1<<Weckerein)))
	{
		oled.draw_ASCI('o',0,0);
	}
	
	//Einstellungsmoeglichekeit
	if ((anzeige&(1<<Einstellungsflag)))
	{
		oled.draw_ASCI(pos+'0',1*charsize,0*charhighte);
	}
}

void fahradschirm(double winkelgeschw, double angle, double weite, double maxgeschwinsigkeit, uint32_t Zeit){
	char buffer[10];
	uint8_t buffersize=0;
	oled.clearFrame();
	header(1);
	bottom(0);
	//Rahmen zeichnen
	oled.drawHLine(0,SSD1306_HEIGHT-9,SSD1306_WIDTH);
	oled.drawHLine(0,numbersmalhight-1,SSD1306_WIDTH/2+5);
	oled.drawHLine(SSD1306_WIDTH/2+5,8,SSD1306_WIDTH/2-5);
	oled.drawVLine(SSD1306_WIDTH/2+5,0,numbersmalhight);
	oled.drawHLine(SSD1306_WIDTH-4*numbersmalsize-1,5*charhighte,4*numbersmalsize+1);
	oled.drawVLine(SSD1306_WIDTH-4*numbersmalsize-1,numbersmalhight,SSD1306_HEIGHT-numbersmalhight);
	oled.drawHLine(SSD1306_WIDTH-4*numbersmalsize-1,3*charhighte,4*numbersmalsize+1);
	
	//lesbare Uhr
	oled.draw_number16x16(rtc.msg_uhr[0]-'0',0*numbersmalsize,0*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[1]-'0',1*numbersmalsize,0*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[3]-'0',2.33*numbersmalsize,0*charhighte);
	oled.draw_number16x16(rtc.msg_uhr[4]-'0',3.33*numbersmalsize,0*charhighte);
	//Ausgabe der Geschwidigkeit
	buffersize=sprintf(buffer,"%3.1f",winkelgeschw);
	for(uint8_t i=((SSD1306_WIDTH/numbersmalsize)-buffersize);i<((SSD1306_WIDTH/numbersmalsize));i++){
		if(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize)]=='.'){
			oled.draw_ASCI(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize)],i*numbersmalsize,4*charhighte);
		}
		else{
			oled.draw_number16x16(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize)]-'0',i*numbersmalsize,3*charhighte);
		}
	}
	//oled.draw_ASCI('k',((SSD1306_WIDTH/charsize-2)*charsize),3*charhighte);
	//oled.draw_ASCI('m',((SSD1306_WIDTH/charsize-1)*charsize),3*charhighte);
	//oled.draw_ASCI('h',((SSD1306_WIDTH/charsize-2)*charsize),4*charhighte);
	//winkelausgabe
	//entfernt aus platz und lesbarkeitsgruenden
	//buffersize=sprintf(buffer,"%3.1f",angle);
	//for (uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,7*charhighte);}
	//Anzeige der Richtung als Alternative
	anzeige_kleinenadel(31,31+8,angle);
	//anzeige der gesammtstrecke
	buffersize=sprintf(buffer,"%.1fm",weite);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize+70,2*charhighte);}
	//anzeige der max geschwindigkeit
	buffersize=sprintf(buffer,"%.1fkm/h",maxgeschwinsigkeit);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize+70,5*charhighte);}
	//anzeige der Fahrtzeit
	buffersize=sprintf(buffer,"%lus",Zeit);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize+70,6*charhighte);}
}

void Gaineinstellen(){
	char buffer[20];
	uint8_t buffersize=0;

	oled.clearFrame();
	buffersize=sprintf(buffer,"1: 1370");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	buffersize=sprintf(buffer,"2: 1090");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,1*charhighte);}
	buffersize=sprintf(buffer,"3:  820");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"4:  660");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	buffersize=sprintf(buffer,"5:  440");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	buffersize=sprintf(buffer,"6:  390");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	buffersize=sprintf(buffer,"7:  330");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,6*charhighte);}
	buffersize=sprintf(buffer,"8:  230");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,7*charhighte);}
}

void anzeige_kompass(double winkel){
	//Baro.READ_Pressure_once();
	//Baro.READ_Temperature();
	oled.clearFrame();
	char buffer[20];
	uint8_t buffersize=0;
	//Uhrzeit
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_uhr[i],65+i*charsize,0*charhighte);}
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_dat[i],65+i*charsize,7*charhighte);}
	//Azeige der Hoehe ueber Altitude
	//buffersize=sprintf(buffer,"H: %.1f m",(double)Baro.altitude(Baro.Press));
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],65+i*charsize,1.33*charhighte);}
	//Anzeige der Temperatur
	//buffersize=sprintf(buffer,"%.1f C",(double)Baro.Tempera);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],65+i*charsize,5*charhighte);}
	//winkel als Zahl ausgeben
	buffersize=sprintf(buffer,"angle=%.0f",winkel);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],65+i*charsize,6*charhighte);}
	//dreieck als Zeiger
	anzeige_nadel(31,31,winkel);
	//Richtung ausgeben
	anzeige_richtung(winkel,80,3.5);	
}

void timerseite(){
	char buffer[20];
	uint8_t buffersize=0;
	oled.clearFrame();
	buffersize=sprintf(buffer,"Timer: ");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	if (pos!=0)
	{
		oled.draw_ASCI(pos+'0',buffersize*charsize,0*charhighte);
	}
	//Zeit berechnung
	uint8_t STD = rtc.Timerzahler/3600;
	uint8_t MIN = (rtc.Timerzahler%3600)/60;
	uint8_t SEC = rtc.Timerzahler%60;
	
	buffer[0]=-STD/3600/10;
	buffer[1]=STD%10;
	buffer[2]=':';
	buffer[3]=MIN/10;
	buffer[4]=MIN%10;
	buffer[5]=':';
	buffer[6]=SEC/10;
	buffer[7]=SEC%10;
	for(uint8_t i=0;i<bitsderrtc;i++){
		if (buffer[i]==':')
		{
			oled.draw_ASCI(buffer[i],i*numbersmalsize,2.5*charhighte);
		}
		else{
			oled.draw_number16x16(buffer[i],i*numbersmalsize,2*charhighte);
		}
	}
}

void Stoppuhrseite(){
	char buffer[20];
	uint8_t buffersize=0;
	oled.clearFrame();
	buffersize=sprintf(buffer,"Stoppuhr: ");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	//Zeit berechnung
	uint8_t STD = rtc.Stoppuhrzahler/3600;
	uint8_t MIN = (rtc.Stoppuhrzahler%3600)/60;
	uint8_t SEC = rtc.Stoppuhrzahler%60;
	
	buffer[0]=-STD/3600/10;
	buffer[1]=STD%10;
	buffer[2]=':';
	buffer[3]=MIN/10;
	buffer[4]=MIN%10;
	buffer[5]=':';
	buffer[6]=SEC/10;
	buffer[7]=SEC%10;
	for(uint8_t i=0;i<bitsderrtc;i++){
		if (buffer[i]==':')
		{
			oled.draw_ASCI(buffer[i],i*numbersmalsize,2.5*charhighte);
		}
		else{
			oled.draw_number16x16(buffer[i],i*numbersmalsize,2*charhighte);
		}
	}
}

void Pressuresensor(){
	//Baro.READ_Pressure_once();
	//Baro.READ_Temperature();
	char buffer[20];
	uint8_t buffersize=0;
	oled.clearFrame();
	header(0);
	bottom(0);
	buffersize=sprintf(buffer,"LPS25H Daten:");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	//buffersize=sprintf(buffer,"%i HPa",Baro.Wertedruck[0]);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	//buffersize=sprintf(buffer,"Hoehe: %.1f m",(double)Baro.altitude(Baro.Press));
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	//buffersize=sprintf(buffer,"%.1f C",(double)Baro.Tempera);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,6*charhighte);}
	//buffersize=sprintf(buffer,"%i",Baro.Wertedruck[1]);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	//buffersize=sprintf(buffer,"%i",Baro.Wertedruck[2]);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
}

void Pokedex(uint8_t Number){
	header(0);
	bottom(0);
	switch (Number)
	{
	case 6:
		oled.draw_glurak();
		break;
	case 25:
		oled.draw_pikachu();
		break;
	default:
		break;
	}
}

void Wanderseite(){
	char buffer[20];
	uint8_t buffersize=0;
	oled.clearFrame();
	header(0);
	bottom(0);
	
	Accelerometer.readtempera();
	Accelerometer.readacc();
	
	//Linse
	//Range: Page 1 to Page 6 (8 bis (7*8-1) 53)
	//ganz rechts SSDWIDTH-(53-8)
	#define libelle	(53.0-8.0)
	oled.drawRectangle(SSD1306_WIDTH-libelle,8,SSD1306_WIDTH-1,53,0);
	oled.drawVLine(SSD1306_WIDTH-libelle/2.0,8,libelle);
	oled.drawHLine(SSD1306_WIDTH-libelle,SSD1306_HEIGHT/2,libelle);
	//Draw libelle uber die Winkel max ist 90 und -90 in beide richtungen
	#define bereichgroese libelle/M_PI*2.0
	#define breite 4
	#define xpos SSD1306_WIDTH-libelle/2.0-breite/2.0
	#define ypos SSD1306_HEIGHT/2.0-breite/2.0
	float rollpix = Accelerometer.roll*bereichgroese;
	float pitchpix = Accelerometer.pitch*bereichgroese;
	if (rollpix<libelle/2.0 && pitchpix<libelle/2.0)
	{
		oled.drawRectangle(xpos-rollpix,ypos+pitchpix,xpos+breite-rollpix,ypos+breite+pitchpix,1);
	}

	//Text debugging
	//buffersize=sprintf(buffer,"r:%.1f",Accelerometer.roll*180.0/M_PI);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	//buffersize=sprintf(buffer,"p:%.1f",Accelerometer.pitch*180.0/M_PI);
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	buffersize=sprintf(buffer,"%i",Accelerometer.achsen_A[0]);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"%i",Accelerometer.achsen_A[1]);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	buffersize=sprintf(buffer,"%i",Accelerometer.achsen_A[2]);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	buffersize=sprintf(buffer,"%u",Accelerometer.Schrittzaehler);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
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

#endif /* SEITEN_H_ */