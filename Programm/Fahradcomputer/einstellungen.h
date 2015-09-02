/*
 * einstellungen.h
 *
 * Created: 05.02.2015 07:07:08
 *  Author: Lüdemann
 */ 


#ifndef EINSTELLUNGEN_H_
#define EINSTELLUNGEN_H_

void zeitsetzen(){
	Minuten = 34;
	Stunden	= 5;
	Sekunden = 0;
	WochenTag=6;
	WStunden[0]=5;
	WMinuten[0]=45;
	WStunden[1]=7;
	WMinuten[1]=45;
	einst(4);
	einst(5);
	einst(0);
}

void init(){
	//ADC
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC) ) {
	}
	(void) ADCW;
	
	//Eingänge
	PORTD |= (1<<PD2) | (1<<PD3) | (1<<PD4);
	
	//Ausgänge
	DDRC = (1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC2) | (1<<PC1);
	PORTC |= (1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC2)  | (1<<PC1);
	PORTC &= ~((1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC2)  | (1<<PC1));
	
	//LCD einstellen und erste Ausgabe
//	PORTC |= (1<PC2);
//	_delay_ms(1);
	lcd_init();
	aus(1,4);
	_delay_ms(1500);
	lcd_clear();
	
	//erste zeiteinstellung
	zeitsetzen();
	
	//Timer 1 Einstelungen
	TCNT1H = 0b11110001;
	TCNT1L = 0b11110000;
	TIMSK |= (1<<TOIE1);
	TCCR1B = (1<<CS10) | (1<<CS12);
	
	//Interupts aktivieren
	sei();
	
	//Timer 0 fuer die Ton ausgabe wird beim Einschalten eingetsellt und muss nicht jetzt eingestellt werden

	//Wecker einschalten
	Wan=0;
	
}

//Funktion um eine Variable, die mit Adreesse uebergeben wird zu erhoehen
//erhoeht wird erst, wenn die richtige Taste gedrueckt wird
void erhoehen(uint8_t *var, uint8_t obergrenze, uint8_t pos, const char *data){
	if(debounce(&PIND,2))
	{
		*var +=1 ;
		if(*var>=obergrenze){
			*var=0;
		}
		aus(1,pos);
		aus(2,0);
		while( *data != '\0' )
		lcd_data( *data++ );
		
	}
}


//return 0 fuer kein Problem
//return 1 fuer falsche Eingabe

uint8_t einst(uint8_t posit){
	uint8_t temp=0;
	//abfrage was eingestellt werden soll
	switch (posit)
	{
	case 0:
		//Einstellen Zeit
		//alles einstellen bis auf Sekunden
		//diese werden auf 0 gesetzt
		aus(1,6);
		aus(2,0);
		lcd_string(Stundenstring);
		while(temp<3)
		{
			if(debounce(&PIND,3))
			{
				temp++;
				lcd_clear();
				aus(1,6);
				aus(2,0);
				switch (temp)
				{
					case 0:
						lcd_string(Stundenstring);
						break;
					case 1:
						lcd_string(Minutenstring);
						break;
					case 2:
						lcd_string(WochenTagstring);
						break;
					default:
						break;
				}
			}
			/*if(Sekunden!=sektemp){
				zeit();
				//ausgabe(0);
				//Es muss eine richtge Ausgabe speziell fuer dieses Einstellung geschrieben werden
				//besonders wichtig ist, dass die alte Ausgaba ersetzt werden muss
			}*/
			switch (temp)
			{
			case 0:
				erhoehen(&Stunden,24,6,Stundenstring);
				break;
			case 1:
				erhoehen(&Minuten,60,6,Minutenstring);
				break;
			case 2:
				//nur jetzt WochenTag
				erhoehen(&WochenTag,7,6,WochenTagstring);
				break;
			default:
				break;
			}
		}
		Sekunden=0;
		return 0;
		break;
	case 1:
		//Einstellen der Lampenstärke
		//default=3
		temp = 0;
		
		if(Sekunden!=sektemp){
			zeit();
		}
		lampenstaerke++;
		if (lampenstaerke>3)
		{
			lampenstaerke=1;
		}
		lcd_clear();
		lcd_home();
		lcd_string(lampenstaerkestring);
		aus(2,0);
		itoa(lampenstaerke,Buffer,10);
		lcd_string(Buffer);
		
		return 0;
		break;
	case 2:
		//Einstellen Sekundenoffset
		//default =-2

		if(Sekunden!=sektemp){
			zeit();
		}
		sekoffset++;
		if (sekoffset>10)
		{
			sekoffset=-10;
		}
		lcd_clear();
		lcd_home();
		lcd_string(sekoffsetstring);
		aus(2,0);
		itoa(sekoffset,Buffer,10);
		lcd_string(Buffer);
		
		return 0;
		break;
	case 3:
		//Einstellen Temperaturoffset
		//default = 0;
		
		if(Sekunden!=sektemp){
			zeit();
		}
			tempoffset++;
			if (tempoffset>10)
			{
				tempoffset=-10;
			}
			lcd_clear();
			lcd_home();
			lcd_string(tempoffsetstring);
			aus(2,0);
			itoa(tempoffset,Buffer,10);
			lcd_string(Buffer);
								
		return 0;
		break;
	case 4: //Wecker einstellen
		temp = 0;
		aus(1,5);
		aus(2,0);
		lcd_string(WStunden0string);
		while (temp<2)
		{
			if (Sekunden!=sektemp)
			{
				zeit();
			}
			if(debounce(&PIND,3))
			{
				temp++;
				aus(1,5);
				aus(2,0);
				switch (temp)
				{
					case 0:
						lcd_string(WStunden0string);
						break;
					case 1:
						lcd_string(WMinuten0string);
						break;
					default:
						break;
				}
			}
			switch (temp)
			{
				case 0:
					erhoehen(&WStunden[0],24,5,WStunden0string);
					break;
				case 1:
					erhoehen(&WMinuten[0],60,5,WMinuten0string);
					break;
			}
			
		}
		//erste Lichteinstellung volle verzoegerung
		Licht1a[0]=WMinuten[0]-weckverzoegerung;
		Licht1a[1]=WStunden[0];
		if (Licht1a[0]>59)
		{
			Licht1a[0]+=60;
			Licht1a[1]-=1;
			if (Licht1a[1]>23)
			{
				Licht1a[1]+=24;
			}
		}
		//zweite Lichteinstellung halbe verzoegerung
		Licht2a[0]=WMinuten[0]-weckverzoegerung/2;
		Licht2a[1]=WStunden[0];
		if (Licht2a[0]>59)
		{
			Licht2a[0]+=60;
			Licht2a[1]-=1;
			if (Licht2a[1]>23)
			{
				Licht2a[1]+=24;
			}
		}
		Wan=0;
		return 0;
		break;

	case 5:
		temp = 0;
		aus(1,7);
		aus(2,0);
		lcd_string(WStunden1string);
		while (temp<2)
		{
			if (Sekunden!=sektemp)
			{
				zeit();
			}
			if(debounce(&PIND,3))
			{
				temp++;
				aus(1,7);
				aus(2,0);
				switch (temp)
				{
					case 0:
					lcd_string(WStunden1string);
					break;
					case 1:
					lcd_string(WMinuten1string);
					break;
					default:
					break;
				}
			}
			switch (temp)
			{
				case 0:
				erhoehen(&WStunden[1],24,7,WStunden1string);
				break;
				case 1:
				erhoehen(&WMinuten[1],60,7,WMinuten1string);
				break;
			}
			
		}
		//Lichteinstellung volle verzoegerung wochenende
		Licht1b[0]=WMinuten[1]-weckverzoegerung;
		Licht1b[1]=WStunden[1];
		if (Licht1b[0]>59)
		{
			Licht1b[0]+=60;
			Licht1b[1]-=1;
			if (Licht1b[1]>23)
			{
				Licht1b[1]+=24;
			}
		}
		//zweite Lichteinstellung halbe verzoegerung
		Licht2b[0]=WMinuten[1]-weckverzoegerung/2;
		Licht2b[1]=WStunden[1];
		if (Licht2b[0]>59)
		{
			Licht2b[0]+=60;
			Licht2b[1]-=1;
			if (Licht2b[1]>23)
			{
				Licht2b[1]+=24;
			}
		}
		Wan=0;
		return 0;
		break;

	case 6:
	//Einstellen Weckerberzoegerung
	//default = 20;
	
		if(Sekunden!=sektemp){
			zeit();
		}
		weckverzoegerung++;
		if (weckverzoegerung>60)
		{
			weckverzoegerung=10;
		}
		lcd_clear();
		lcd_home();
		lcd_string(weckverzoegerungstring);
		aus(2,0);
		itoa(weckverzoegerung,Buffer,10);
		lcd_string(Buffer);
		return 0;
		break;

	default:
		return 1;
		break;
	}
}



#endif /* EINSTELLUNGEN_H_ */