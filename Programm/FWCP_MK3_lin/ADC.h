﻿/*
 * ADC.h
 *
 * Created: 05.02.2015 07:05:40
 *  Author: Lüdemann
 */ 


#ifndef ADC_H_
#define ADC_H_

uint16_t ADC_Read( uint8_t channel )
{
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC) ) {
	}
	return ADCW;
}

uint16_t ADC_Read_Avg( uint8_t channel, uint8_t nsamples )
{
	uint32_t sum = 0;
	
	for (uint8_t i = 0; i < nsamples; ++i ) {
		sum += ADC_Read( channel );
	}
	
	return (uint16_t)( sum / nsamples );
}

double batterie;

#ifndef BATMIN
#define BATMIN	3.0
#endif
#ifndef BATMAX
#define BATMAX	4.2
#endif


int8_t Batteriestatus(){
	uint16_t ADCwert=ADC_Read(0);
	static double bat_mavg[5];
	for(uint8_t i=0; i<4; i++){bat_mavg[i+1] = bat_mavg[i];}
	double batt_now = (ADCwert/1023.0)*3.29*SPANNUNGSTEILER;
	bat_mavg[0] = batt_now;
	batterie = 0;
	for(uint8_t i=0; i<5; i++){batterie += bat_mavg[i]/5.;}
	float stat = ((batterie-BATMIN)/(BATMAX-BATMIN)*100);
	uint8_t temp =(uint8_t) (stat*10);
	if (temp%10>=5)
	{
		stat = (uint8_t)stat+1;
	}
	else
	{
		stat = (uint8_t)stat;
	}
	
	return stat;
}

#endif /* ADC_H_ */
