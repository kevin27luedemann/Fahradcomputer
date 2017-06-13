/*
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

#define numberavv 5.0
int8_t StatAVV(int8_t stat){
	static int8_t mavv[(uint8_t)numberavv];
	int16_t summe_avv = 0;
	
	for (uint8_t i=numberavv; i>=1; i--)
	{
		mavv[i] = mavv[i-1];
	}
	mavv[0] = stat;
	
	for (uint8_t i=0; i<numberavv; i++)
	{
		summe_avv += mavv[i];
	}
	
	return summe_avv/numberavv;
}

int8_t Batteriestatus(){
	uint16_t ADCwert=ADC_Read(0);
	batterie = (ADCwert/1023.0)*3.29*SPANNUNGSTEILER;
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
	
	return StatAVV(stat);
}

#endif /* ADC_H_ */