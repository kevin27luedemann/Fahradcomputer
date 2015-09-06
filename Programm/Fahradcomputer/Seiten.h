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
	buffersize=sprintf(buffer,"Version: %.2f",VERSIONSNUMMER);
	for(uint8_t i=0; i < buffersize;i++){
		oled.draw_ASCI(buffer[i],i*charsize,32);

	}
	oled.sendFrame();
	oled.clearFrame();
	for (uint8_t i=0;i<10;i++)
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
		default:
			break;
	}
}

void anzeige_richtung(float winkel){
	//finden der Ziffer fuer die Anzeige
	if ((winkel<45/2) || (winkel == 360-45/2))
	{
		oled.draw_ASCI('N',80,3.5*charhighte);
	}
	else if (winkel<90-45/2)
	{
		oled.draw_ASCI('N',80-charsize/2,3.5*charhighte);
		oled.draw_ASCI('O',80+charsize/2,3.5*charhighte);
	}
	else if (winkel<135-45/2)
	{
		oled.draw_ASCI('O',80,3.5*charhighte);
	}
	else if (winkel<180-45/2)
	{
		oled.draw_ASCI('S',80-charsize/2,3.5*charhighte);
		oled.draw_ASCI('O',80+charsize/2,3.5*charhighte);
	}
	else if (winkel<225-45/2)
	{
		oled.draw_ASCI('S',80,3.5*charhighte);
	}
	else if (winkel<270-45/2)
	{
		oled.draw_ASCI('S',80-charsize/2,3.5*charhighte);
		oled.draw_ASCI('W',80+charsize/2,3.5*charhighte);
	}
	else if (winkel<315-45/2)
	{
		oled.draw_ASCI('W',80,3.5*charhighte);
	}
	else if (winkel<360-45/2)
	{
		oled.draw_ASCI('N',80-charsize/2,3.5*charhighte);
		oled.draw_ASCI('W',80+charsize/2,3.5*charhighte);
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

/*
void bottom(){
	//	char buffer[20];
	//	uint8_t buffersize=0;
	for(uint8_t i=((SSD1306_WIDTH/charsize)-8);i<(SSD1306_WIDTH/charsize);i++){
		draw_ASCI(msg_dat[i-((SSD1306_WIDTH/charsize)-8)],i*charsize,7*charhighte);
	}
}
*/
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
	//buffersize=sprintf(buffer,"7:Uhr einstellen");
	//for(uint8_t i=0;i<buffersize;i++){draw_ASCI(oled.buffer[i],i*charsize,6*charhighte);}
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
	//buffersize=sprintf(buffer,"4:Uhr");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	//buffersize=sprintf(buffer,"5:Kompass kalib.");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
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
	buffersize=sprintf(buffer,"4:Alarm");
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	//buffersize=sprintf(buffer,"5:Kompass kalib.");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	//buffersize=sprintf(buffer,"6:HMC5883L Gain");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,5*charhighte);}
	//buffersize=sprintf(buffer,"7:Uhr einstellen");
	//for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,6*charhighte);}
}

void uhranzeigen(){
	uint8_t buffersize;
	char Buffer[20];
	oled.clearFrame();
	oled.analog(rtc.Stunden,rtc.Minuten,rtc.Sekunden);
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_uhr[i],65+i*charsize,0);}
	buffersize=sprintf(Buffer,"%i%%",Batteriestatus());
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(Buffer[i],65+i*charsize,2*charhighte);}
	buffersize=sprintf(Buffer,"%.3fV",batterie);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(Buffer[i],65+i*charsize,3*charhighte);}
}

/*
unsigned int stoppuhr(){
	uint8_t sek=0;
	uint8_t min=0;
	uint8_t stun=0;
	uint8_t Zeitausgabe[12];
	Zeitausgabe[4]=0;
	Zeitausgabe[5]=0;
	Zeitausgabe[6]=':';
	Zeitausgabe[7]=0;
	Zeitausgabe[8]=0;
	Zeitausgabe[9]=':';
	Zeitausgabe[10]=0;
	Zeitausgabe[11]=0;
	
	//erstausgabe
	oled.clearFrame();
	for (uint8_t i=4;i<12;i++)
	{
		if (Zeitausgabe[i]==':' || Zeitausgabe[i]=='.')
		{
			oled.draw_ASCI(Zeitausgabe[i],(i-4)*numbersmalsize,2.5*charhighte);
		}
		else{
			oled.draw_number16x16(Zeitausgabe[15-i],(i-4)*numbersmalsize,2*charhighte);
		}
	}
	oled.draw_number16x16(Zeitausgabe[2],4*numbersmalsize,3*charhighte);
	oled.sendFrame();
	
	uint8_t taste='A';
	uint8_t spezflag=0;
	while (taste!='5')
	{
		if((rtc.interupts & (1<<hundinterupt))){
			rtc.zeit();
			rtc.interupts&=~(1<<hundinterupt);	//Flag zureucksetzen
		}
		if ((rtc.interupts & (1<<sekundeninterupt)) && (spezflag&(1<<stoppbit)))
		{
			sek++;
			//Ausgabe der Zeit nach der Berechung
			Zeitausgabe[10]=sek/10;
			Zeitausgabe[11]=sek%10;
			oled.clearFrame();
			for (uint8_t i=4;i<12;i++)
			{
				if (Zeitausgabe[i]==':' || Zeitausgabe[i]=='.')
				{
					oled.draw_ASCI(Zeitausgabe[i],(i-4)*numbersmalsize,2.5*charhighte);
				}
				else{
					oled.draw_number16x16(Zeitausgabe[i],(i-4)*numbersmalsize,2*charhighte);	
				}
			}
			oled.sendFrame();
			rtc.interupts &=~(1<<sekundeninterupt);
		}
		if (sek>=60)
		{
			sek=0;
			min++;
			Zeitausgabe[7]=min/10;
			Zeitausgabe[8]=min%10;
			if (min>=60)
			{
				min=0;
				stun++;
				Zeitausgabe[4]=stun/10;
				Zeitausgabe[5]=stun%10;
				if (stun>=24)
				{
					stun=0;
				}
			}
		}
		
		taste=Tastatur.Taster();
		
		if (taste=='#' && !(spezflag&(1<<stoppbit)))
		{
			spezflag |= (1<<stoppbit);
		}
		else if (taste=='#' && (spezflag&(1<<stoppbit)))
		{
			spezflag &= ~(1<<stoppbit);
		}
		else if (taste=='*')
		{
			spezflag &= ~(1<<stoppbit);
			sek=0;
			min=0;
			stun=0;
			Zeitausgabe[4]=0;
			Zeitausgabe[5]=0;
			Zeitausgabe[6]=':';
			Zeitausgabe[7]=0;
			Zeitausgabe[8]=0;
			Zeitausgabe[9]=':';
			Zeitausgabe[10]=0;
			Zeitausgabe[11]=0;
				
			//erstausgabe
			oled.clearFrame();
			for (uint8_t i=4;i<12;i++)
			{
				if (Zeitausgabe[i]==':' || Zeitausgabe[i]=='.')
				{
					oled.draw_ASCI(Zeitausgabe[i],(i-4)*numbersmalsize,2.5*charhighte);
				}
				else{
					oled.draw_number16x16(Zeitausgabe[15-i],(i-4)*numbersmalsize,2*charhighte);
				}
			}
			oled.sendFrame();
		}
		
	}
	
	return (24*stun+60*min+sek);
}*/

void fahradschirm(double winkelgeschw, double angle){
	char buffer[10];
	uint8_t buffersize=0;
	oled.clearFrame();
	header(0);
	//bottom();
	//Ausgabe der Geschwidigkeit
	buffersize=sprintf(buffer,"%3.1f",winkelgeschw);
	for(uint8_t i=((SSD1306_WIDTH/numbersmalsize)-buffersize-1);i<((SSD1306_WIDTH/numbersmalsize)-1);i++){
		if(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize-1)]=='.'){
			oled.draw_ASCI(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize-1)],i*numbersmalsize,4*charhighte);
		}
		else{
			oled.draw_number16x16(buffer[i-((SSD1306_WIDTH/numbersmalsize)-buffersize-1)]-'0',i*numbersmalsize,3*charhighte);
		}
	}
	oled.draw_ASCI('k',((SSD1306_WIDTH/charsize-2)*charsize),3*charhighte);
	oled.draw_ASCI('m',((SSD1306_WIDTH/charsize-1)*charsize),3*charhighte);
	oled.draw_ASCI('h',((SSD1306_WIDTH/charsize-2)*charsize),4*charhighte);
	//winkelausgabe
	buffersize=sprintf(buffer,"%3.1f",angle);
	for (uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],i*charsize,7*charhighte);}
	anzeige_kleinenadel(31,31,angle);
}

/*
void calibrate_kompass(){
	char buffer[20];
	uint8_t buffersize=0;
	int16_t max[3]={0,0,0}, min[3]={0,0,0};
	uint8_t taste=0;
	bool refresh=false;
	kompass.HMC5883L_command(0x02,HMCCONTINUOUSMODE);
	while(taste!='*'){
		taste=Taster();
		if((interupts & (1<<hundinterupt))){
			zeit();
			interupts&=~(1<<hundinterupt);	//Flag zureucksetzen
			kompass.HMC5883L_readHeading();
			for(uint8_t i=0;i<3;i++){
				if(achsen[i]>max[i]){
					max[i]=achsen[i];
					refresh=true;
				}
				else if(achsen[i]<min[i]){
					min[i]=achsen[i];
					refresh=true;
				}
			}
		}
		if (refresh)
		{
			clearFrame();
			buffersize=sprintf(buffer,"Kallibration");
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,0*charhighte);}
			buffersize=sprintf(buffer,"Max_x:%i",max[0]);
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,1*charhighte);}
			buffersize=sprintf(buffer,"Min_x:%i",min[0]);
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,2*charhighte);}
			buffersize=sprintf(buffer,"Max_y:%i",max[1]);
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,3*charhighte);}
			buffersize=sprintf(buffer,"Min_y:%i",min[1]);
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,4*charhighte);}
			buffersize=sprintf(buffer,"Max_z:%i",max[2]);
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,5*charhighte);}
			buffersize=sprintf(buffer,"Min_z:%i",min[2]);
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,6*charhighte);}
			sendFrame();
			refresh=false;
		}
		if(taste=='#'){
			for(uint8_t i=0;i<3;i++){
				min[i]=0;
				max[i]=0;
			}
		}
	}
	HMC5883L_command(0x02,HMCIDLEMODE);
	for (uint8_t i=0;i<3;i++)
	{
		offset[i]= (max[0]+min[0])/2;
	}
}
*/
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
	oled.clearFrame();
	char buffer[20];
	uint8_t buffersize=0;
	//Uhrzeit
	for(uint8_t i=0;i<bitsderrtc;i++){oled.draw_ASCI(rtc.msg_uhr[i],65+i*charsize,0);}
	//winkel als Zahl ausgeben
	buffersize=sprintf(buffer,"angle=%.0f",winkel);
	for(uint8_t i=0;i<buffersize;i++){oled.draw_ASCI(buffer[i],65+i*charsize,7*charhighte);}
	//dreieck als Zeiger
	anzeige_nadel(31,31,winkel);
	//Richtung ausgeben
	anzeige_richtung(winkel);	
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
		
	buffer[0]=zaehler.Stunden/10;
	buffer[1]=zaehler.Stunden%10;
	buffer[2]=':';
	buffer[3]=zaehler.Minuten/10;
	buffer[4]=zaehler.Minuten%10;
	buffer[5]=':';
	buffer[6]=zaehler.Sekunden/10;
	buffer[7]=zaehler.Sekunden%10;
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
	
	buffer[0]=stoppuhr.Stunden/10;
	buffer[1]=stoppuhr.Stunden%10;
	buffer[2]=':';
	buffer[3]=stoppuhr.Minuten/10;
	buffer[4]=stoppuhr.Minuten%10;
	buffer[5]=':';
	buffer[6]=stoppuhr.Sekunden/10;
	buffer[7]=stoppuhr.Sekunden%10;
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
						rtc.Tag=0;
						rtc.Monat=0;
						rtc.Jahr=0;
						pos=3;
						buffersize=sprintf(buffer,"Tag falsch");
						for(uint8_t i = 0; i<buffersize;i++){
							oled.draw_ASCI(buffer[i],charsize*i,7*8);
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
	oled.clearFrame();
	rtc.RTCstart();
}

#endif /* SEITEN_H_ */