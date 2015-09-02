/*
 * Taschenrechner.h
 *
 *  Created on: 18.08.2015
 *      Author: kevin
 */

#ifndef TASCHENRECHNER_H_
#define TASCHENRECHNER_H_

uint32_t zehner(uint8_t zahler){
	if(zahler==0){
		return 1;
	}
	else{
		return 10*zehner(--zahler);
	}
}

double Flieskommazahleingabe(){
	uint8_t taste='A', pos=0;
	char Zahl1[20], buffer[20];
	uint8_t buffersize;
	double zahla = 0;
	bool komma=false;
	uint8_t kommapos=0;
	clearFrame();
	buffersize=sprintf(buffer,"Zahl eintippen:");
	bool update = true;

	while(taste!='*'){
		if(update){
			for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,0);}
			sendFrame();
			update=false;
		}
		if ((interupts & (1<<hundinterupt)))
		{
			rtc.zeit();
			interupts&=~(1<<hundinterupt);	//Flag zureucksetzen
		}
		taste=Taster();
		if(taste!='A'){
			if(taste>='0' && taste<='9'){
				zahla=0;
				Zahl1[pos]=taste;
				for(uint8_t i=0; i<=pos; i++){
					if(i==kommapos && komma){
						continue;
					}
					else if (i>kommapos && komma) {
						zahla+=(Zahl1[i]-'0')/(double)zehner(i-kommapos);
					}
					else if (komma){
						zahla+=(Zahl1[i]-'0')*zehner(kommapos-i-1);
					}
					else{
						zahla+=(Zahl1[i]-'0')*zehner(pos-i);
					}

				}
				clearFrame();
				for(uint8_t i=0; i<=pos;i++){
					draw_ASCI(Zahl1[i],i*charsize,charhighte);
				}
				update=true;
				pos++;
			}
			if(taste=='#' && !komma){
				komma=true;
				Zahl1[pos]='.';
				kommapos=pos;
				pos++;
			}
			if(taste=='*'){
				clearFrame();
				buffersize=sprintf(buffer,"1:PI  2:PI/2  3:PI/4");
				for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,0*charhighte);}
				buffersize=sprintf(buffer,"4:e  5:Loeschen");
				for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,1*charhighte);}
				buffersize=sprintf(buffer,"*:fertig");
				for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,2*charhighte);}
				sendFrame();
				clearFrame();
				buffersize=sprintf(buffer,"Zahl eintippen:");
				bool halt=true;
				while(halt){
					taste=Taster();
					if ((interupts & (1<<hundinterupt)))
					{
						zeit();
						interupts&=~(1<<hundinterupt);	//Flag zureucksetzen
					}
					switch (taste) {
						case '1':
							zahla=M_PI;
							halt=false;
							taste='*';
							break;
						case '2':
							zahla=M_PI_2;
							halt=false;
							taste='*';
							break;
						case '3':
							zahla=M_PI_4;
							halt=false;
							taste='*';
							break;
						case '4':
							zahla=M_E;
							halt=false;
							taste='*';
							break;
						case '5':
							halt=false;
							taste='A';
							pos=0;
							komma=false;
							kommapos=0;
							update=true;
							break;
						case '*':
							halt=false;
							break;
						default:
							break;
					}
				}
			}
		}
	}
	return zahla;
}

double operation(double zahl){
	uint8_t taste='A';
	char buffer[20];
	uint8_t buffersize=0;
	uint8_t oper[5] = {' ',' ',' ',' ',' '};
	double Zahl2=0, ergeb=0;

	clearFrame();
	buffersize=sprintf(buffer,"1:+  2:-  3:*  4:/");
	for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	buffersize=sprintf(buffer,"5:^2  6:^-1  7:sqrt");
	for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,1*charhighte);}
	buffersize=sprintf(buffer,"8:sin  9:cos  0:tan");
	for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,2*charhighte);}
	buffersize=sprintf(buffer,"*:weiter");
	for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	bool update = true;
	while(taste=='A'){
		if(update){
			sendFrame();
			update=false;
		}
		if ((interupts & (1<<hundinterupt)))
		{
			zeit();
			interupts&=~(1<<hundinterupt);	//Flag zureucksetzen
		}
		taste=Taster();
		switch (taste) {
			case '1':
				oper[0] = '+';
				Zahl2 = Flieskommazahleingabe();
				ergeb = zahl+Zahl2;
				break;
			case '2':
				oper[0]='-';
				Zahl2 = Flieskommazahleingabe();
				ergeb = zahl-Zahl2;
				break;
			case '3':
				oper[0]='*';
				Zahl2 = Flieskommazahleingabe();
				ergeb = zahl*Zahl2;
				break;
			case '4':
				oper[0]='/';
				Zahl2 = Flieskommazahleingabe();
				if(Zahl2==0){
					//TODO mache etwas, wenn durch null geteilt
				}
				else {
					ergeb = zahl/Zahl2;
				}
				break;
			case '5':
				oper[0]='^';
				oper[1]='2';
				ergeb = zahl*zahl;
				break;
			case '6':
				oper[0]='^';
				oper[1]='-';
				oper[2]='1';
				if(zahl==0){
					//TODO mache etwas, wenn durch null geteilt
				}
				else{
					ergeb=1/zahl;
				}
				break;
			case '7':
				oper[0]='s';
				oper[1]='q';
				oper[2]='r';
				oper[3]='t';
				if(zahl<0){
					//TODO fuehre imaginaere Zahlen ein
				}
				else {
					ergeb=sqrt(zahl);
				}
				break;
			case '8':
				oper[0]='s';
				oper[1]='i';
				oper[2]='n';
				ergeb=sin(zahl);
				break;
			case '9':
				oper[0]='c';
				oper[1]='o';
				oper[2]='s';
				ergeb=cos(zahl);
				break;
			case '0':
				oper[0]='t';
				oper[1]='a';
				oper[2]='n';
				ergeb=tan(zahl);
				break;
			case '*':
				taste='A';
				break;
			default:
				break;
		}
	}
	clearFrame();
	buffersize=sprintf(buffer,"%f",zahl);
	for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,0*charhighte);}
	for(uint8_t i=0;i<5;i++){draw_ASCI(oper[i],i*charsize,1*charhighte);}
	if(oper[1]==' '){
		buffersize=sprintf(buffer,"%f",Zahl2);
		for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,2*charhighte);}
		draw_ASCI('=',0,3*charhighte);
		buffersize=sprintf(buffer,"%f",ergeb);
		for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,4*charhighte);}
	}
	else {
		draw_ASCI('=',0,2*charhighte);
		buffersize=sprintf(buffer,"%f",ergeb);
		for(uint8_t i=0;i<buffersize;i++){draw_ASCI(buffer[i],i*charsize,3*charhighte);}
	}
	sendFrame();
	return ergeb;
}

#endif /* TASCHENRECHNER_H_ */
