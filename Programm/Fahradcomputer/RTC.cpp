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
	Sekunden=0;
	HundSekunden=0;
	Minuten=0;
	Stunden=0;
	Tag=0;
	Monat=0;
	Jahr=0;
	WTag=0;
	WMinuten=40;
	WStunden=10;
	for(uint8_t i=0;i<bitsderrtc;i++){
		msg_uhr[i]=0;
		msg_dat[i]=0;
	}
	//Einstellungen des Ausgabebuffers
	msg_uhr[2]=':';
	msg_uhr[5]=':';
	msg_dat[2]='.';
	msg_dat[5]='.';
	ausgabezeitneu();
	ausgabedatumneu();
} //RTC

// default destructor
RTC::~RTC()
{
} //~RTC

uint8_t RTC::zeit_hund(){
	//HundSekunden++; sitzt im Vektor um Taster besser entprellen zu koennen
	if (HundSekunden>=0)
	{
		HundSekunden = 0;
		Sekunden++;
		interupts |= (1<<sekundeninterupt);
		if (Sekunden >= 60 && Sekunden <=200){
			Sekunden = 0;
			Minuten++;
			interupts |= (1<<minuteninterupt);
			if(Minuten >= 60){
				Minuten = 0;
				Stunden++;
			}
			if(Stunden >= 24){
				Stunden = 0;
				kalender();
			}
		}
		Wecker();
	}
	ausgabezeitneu();
	return 0;
}

uint8_t RTC::zeit(){
	//Sekunden++; sitzt im Vektor um Taster besser entprellen zu koennen
	if (Sekunden >= 60 && Sekunden <=200){
		Sekunden = 0;
		Minuten++;
		interupts |= (1<<minuteninterupt);
		if(Minuten >= 60){
			Minuten = 0;
			Stunden++;
			if(Stunden >= 24){
				Stunden = 0;
				Tag++;
				kalender();
			}
		}
		Wecker();
	}
	ausgabezeitneu();
	return 0;
}

void RTC::kalender(){
	if (Monat==4||Monat==6||Monat==9||Monat==11)
	{
		if (Tag>=31)
		{
			Tag=1;
			Monat++;
			if (Monat>=13)
			{
				Monat=1;
				Jahr++;
			}
		}
	}
	else if (Monat==2)
	{
		if (Jahr%4==0)
		{
			if (Tag>=30)
			{
				Tag=1;
				Monat++;
				if (Monat>=13)
				{
					Monat=1;
					Jahr++;
				}
			}
		}
		else{
			if (Tag>=29)
			{
				Tag=1;
				Monat++;
				if (Monat>=13)
				{
					Monat=1;
					Jahr++;
				}
			}
		}
	}
	else{
		if (Tag>=32)
		{
			Tag=1;
			Monat++;
			if (Monat>=13)
			{
				Monat=1;
				Jahr++;
			}
		}
	}
	ausgabedatumneu();
}

void RTC::Wecker(){
	if ((interupts&(1<<Weckerein))&&!(interupts&(1<<Weckeractiv)))
	{
		if (WStunden==Stunden)
		{
			if (WMinuten==Minuten)
			{
				interupts|=(1<<Weckeractiv);
			}
		}
	}
}


void RTC::ausgabezeitneu(){
	msg_uhr[0]='0'+Stunden/10;
	msg_uhr[1]='0'+Stunden%10;
	msg_uhr[3]='0'+Minuten/10;
	msg_uhr[4]='0'+Minuten%10;
	msg_uhr[6]='0'+Sekunden/10;
	msg_uhr[7]='0'+Sekunden%10;
}
void RTC::ausgabedatumneu(){
	msg_dat[0]='0'+Tag/10;
	msg_dat[1]='0'+Tag%10;
	msg_dat[3]='0'+Monat/10;
	msg_dat[4]='0'+Monat%10;
	msg_dat[6]='0'+Jahr/10;
	msg_dat[7]='0'+Jahr%10;
}

void RTC::dummyeinst(){
	Stunden=10;
	Minuten=30;
	Tag=7;
	Monat=9;
	Jahr=15;
	ausgabedatumneu();
	ausgabezeitneu();
}

void RTC::RTCstart(){
	//Einstellungen des Ausgabebuffers
	msg_uhr[0]='0';
	msg_uhr[1]='0';
	msg_uhr[2]=':';
	msg_uhr[3]='0';
	msg_uhr[4]='0';
	msg_uhr[5]=':';
	msg_uhr[6]='0';
	msg_uhr[7]='0';
	//Timer2 als 1ms Timer fuer die RTC
	//asynchron mit 32.768 kHz (noch synchrom)
	TCNT2 = TIMER2RTCTIME;
	ASSR |= (1<<AS2);
	TIMSK2 = (1<<TOIE2);
	TCCR2B = (1<<CS22) | (1<<CS20);		//Vorteiler 256
}

void RTC::RTCstop(){
	TCCR2B &= ~((1<<CS22) | (1<<CS21));
}

uint8_t RTC::timer(){
	if (Sekunden>60)
	{
		Sekunden=59;
		Minuten--;
		if (Minuten>60)
		{
			Minuten=59;
			Stunden--;
			if (Stunden>24)
			{
				Stunden=0;
				Minuten=0;
				Sekunden=0;
				return 1;
			}
		}
	}
	return 0;
}
