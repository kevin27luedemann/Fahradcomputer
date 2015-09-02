/*
 * Zeit.h
 *
 * Created: 25.08.2015 08:04:54
 *  Author: kevin
 */ 


#ifndef ZEIT_H_
#define ZEIT_H_

uint8_t Stunden, Minuten, Sekunden, HundSekunden;
uint8_t Tag, Monat, Jahr; //Jahr=Jahr ab 2000 als integer
//char msg_dat[bitsderrtc];
char msg_uhr[bitsderrtc];

uint8_t kalender(){
	Tag++;
	return 0;
}

uint8_t zeit(){
	//HundSekunden++; sitzt im Vektor um Taster besser entprellen zu koennen
	if (HundSekunden>=100)
	{
		HundSekunden = 0;
		Sekunden++;
		interupts |= 0x02;
		if (Sekunden >= 60 && Sekunden <=200){
			Sekunden = 0;
			Minuten++;
			if(Minuten >= 60){
				Minuten = 0;
				Stunden++;
			}
			if(Stunden >= 24){
				Stunden = 0;
				kalender();
			}
		}
	}
	msg_uhr[0]='0'+Stunden/10;
	msg_uhr[1]='0'+Stunden%10;
	msg_uhr[3]='0'+Minuten/10;
	msg_uhr[4]='0'+Minuten%10;
	msg_uhr[6]='0'+Sekunden/10;
	msg_uhr[7]='0'+Sekunden%10;
/*
	msg_dat[0]='0'+GetTime.Days/10;
	msg_dat[1]='0'+GetTime.Days%10;
	msg_dat[3]='0'+GetTime.Month/10;
	msg_dat[4]='0'+GetTime.Month%10;
	msg_dat[6]='0'+(GetTime.Year-2000)/10;
	msg_dat[7]='0'+(GetTime.Year-2000)%10;
	*/
	return 0;
}

void uhreinstellen(){
	clearFrame();
	
	char buffer[20];
	uint8_t buffersize;
	uint8_t pos=0;
	while(pos<11){
		buffersize=sprintf(buffer,"Uhreinstellung");
		for(uint8_t i = 0; i<buffersize;i++){
			draw_ASCI(buffer[i],charsize*i,0);
		}
		buffersize=sprintf(buffer,"--------------");
		for(uint8_t i = 0; i<buffersize;i++){
			draw_ASCI(buffer[i],charsize*i,8);
		}
		//buffersize=sprintf(buffer, "%u:%u %u.%u.%u",Stunden,Minuten,Tag,Monat,Jahr);
		buffer[0]=Stunden/10;
		buffer[1]=Stunden%10;
		buffer[2]=':';
		buffer[3]=Minuten/10;
		buffer[4]=Minuten%10;
		buffer[5]=' ';
		buffer[6]=Tag/10;
		buffer[7]=Tag%10;
		buffer[8]='.';
		buffer[9]=Monat/10;
		buffer[10]=Monat%10;
		buffer[11]='.';
		buffer[12]=Jahr/10;
		buffer[13]=Jahr%10;
		buffersize=14;
		for(uint8_t i = 0; i<5;i++){
			if(i!=2){
				draw_number16x16(buffer[i],i*numbersmalsize,2*charhighte);
			}
			else {
				draw_ASCI(buffer[i],i*numbersmalsize+numbersmalsize/2,2*charhighte+charhighte/2);
			}
		}
		for(uint8_t i = 6; i<buffersize;i++){
			if(i!=8 && i!=11){
				draw_number16x16(buffer[i],i*numbersmalsize-6*numbersmalsize,4*charhighte);
			}
			else {
				draw_ASCI(buffer[i],i*numbersmalsize+numbersmalsize/2-6*numbersmalsize,5*charhighte);
			}
		}

		sendFrame();
		clearFrame();
		switch (pos) {
			case 0:
				Stunden=ZahlenausTastatur()*10;
				if(Stunden>=24){
					Stunden=0;
					pos=-1;
					buffersize=sprintf(buffer,"Stunden falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 1:
				Stunden+=ZahlenausTastatur();
				if(Stunden>=24){
					Stunden=0;
					pos=-1;
					buffersize=sprintf(buffer,"Stunden falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 2:
				Minuten=ZahlenausTastatur()*10;
				if(Minuten>=60){
					Minuten=0;
					pos=1;
					buffersize=sprintf(buffer,"Minuten falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 3:
				Minuten+=ZahlenausTastatur();
				if(Minuten>=60){
					Minuten=0;
					pos=1;
					buffersize=sprintf(buffer,"Minuten falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 4:
				Tag=ZahlenausTastatur()*10;
				if(Tag>=32){
					Tag=0;
					pos=3;
					buffersize=sprintf(buffer,"Tag falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 5:
				Tag+=ZahlenausTastatur();
				if(Tag>=32){
					Tag=0;
					pos=3;
					buffersize=sprintf(buffer,"Tag falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 6:
				Monat=ZahlenausTastatur()*10;
				if(Monat>=13){
					Monat=0;
					pos=5;
					buffersize=sprintf(buffer,"Monat falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 7:
				Monat+=ZahlenausTastatur();
				if(Monat>=13){
					Monat=0;
					pos=5;
					buffersize=sprintf(buffer,"Monat falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				if(Monat==4||Monat==6||Monat==9||Monat==11){
					if(Tag>=31){
						Tag=0;
						Monat=0;
						pos=3;
						buffersize=sprintf(buffer,"Tag falsch");
						for(uint8_t i = 0; i<buffersize;i++){
							draw_ASCI(buffer[i],charsize*i,7*8);
						}
					}
				}
				else if(Monat==2){
					if(Tag>=30){
						Tag=0;
						Monat=0;
						pos=3;
						buffersize=sprintf(buffer,"Tag falsch");
						for(uint8_t i = 0; i<buffersize;i++){
							draw_ASCI(buffer[i],charsize*i,7*8);
						}
					}
				}
				break;
			case 8:
				Jahr=ZahlenausTastatur()*10;
				if(Jahr>=50){
					Jahr=0;
					pos=7;
					buffersize=sprintf(buffer,"Jahr falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				break;
			case 9:
				Jahr+=ZahlenausTastatur();
				if(Jahr>=50){
					Jahr=0;
					pos=7;
					buffersize=sprintf(buffer,"Jahr falsch");
					for(uint8_t i = 0; i<buffersize;i++){
						draw_ASCI(buffer[i],charsize*i,7*8);
					}
				}
				if(Monat==2){
					if(Jahr%4==0){
						Tag=0;
						Monat=0;
						Jahr=0;
						pos=3;
						buffersize=sprintf(buffer,"Tag falsch");
						for(uint8_t i = 0; i<buffersize;i++){
							draw_ASCI(buffer[i],charsize*i,7*8);
						}
					}
					else{
						if(Tag>=29){
							Tag=0;
							Monat=0;
							Jahr=0;
							pos=3;
							buffersize=sprintf(buffer,"Tag falsch");
							for(uint8_t i = 0; i<buffersize;i++){
								draw_ASCI(buffer[i],charsize*i,7*8);
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
	
	Sekunden		= 0;
	HundSekunden	= 0;
	clearFrame();
}



#endif /* ZEIT_H_ */