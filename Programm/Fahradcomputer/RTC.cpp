/* 
* RTC.cpp
*
* Created: 02.09.2015 12:17:51
* Author: kevin
*/


#include "RTC.h"

// default constructor
RTC::RTC()
{
} //RTC

// default destructor
RTC::~RTC()
{
	Sekunden=0;
	HundSekunden=0;
	Minuten=0;
	Stunden=0;
	Tag=0;
	Monat=0;
	Jahr=0;
	for(uint8_t i=0;i<bitsderrtc;i++){
		msg_uhr[i]=0;
	}
	//Einstellungen des Ausgabebuffers
	msg_uhr[2]=':';
	msg_uhr[5]=':';
	//msg_dat[2]='.';
	//msg_dat[5]='.';
} //~RTC

uint8_t RTC::zeit(){
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

void RTC::kalender(){
	Tag++;
}

void RTC::dummyeinst(){
	Stunden=10;
	Minuten=30;
	Tag=27;
	Monat=8;
	Jahr=15;
}

void RTC::RTCstart(){
	//Einstellungen des Ausgabebuffers
	msg_uhr[2]=':';
	msg_uhr[5]=':';
	//Timer2 als 1ms Timer fuer die RTC
	//noch synchrom
	TCNT2 = TIMER2RTCTIME;
	TIMSK2 = (1<<TOIE2);
	TCCR2B = (1<<CS22) | (1<<CS21);
}

void RTC::RTCstop(){
	TCCR2B &= ~((1<<CS22) | (1<<CS21));
}