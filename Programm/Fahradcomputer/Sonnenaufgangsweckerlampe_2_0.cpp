#include "definitionen.h"
#include "LCD.h"
#include "ADC.h"
#include "einstellungen.h"
#include "zeit.h"
#include "menue.h"
#include "ausgabe.h"

ISR(TIMER0_OVF_vect){
	TCNT0 = 255-35;
	if (!(PORTC & (1<<PC1)))
	{
		lichteinaus(3);
		PORTC |= (1<<PC1);
	}
	else
	{
		lichteinaus(0);
		PORTC &= ~(1<<PC1);
	}
}

ISR(TIMER1_OVF_vect){
	TCNT1H = 0b11110001;
	TCNT1L = 0b11110000;
	Sekunden++;
}

int main(void)
{
	init();
	
	while(1)
	{
		if (Sekunden!=sektemp)
		{
			zeit();
			if (displaystat)
			{
				aus(1,3);	//obere Zeile
				aus(2,1);	//untere zeile
				if (Wan!=1)
				{
					//Aussetzen, des Ausschalten, da keine moeglichkeit das Display wirklich auszuschalten
					//displayoff++;
				}
				else if (!displaystat && Wan ==1)
				{
					//Display wieder anschalten, falls noch nicht angeschaltet
					PORTC |= (1<<PC2); //display einsschalten und neue anzeige starten
					lcd_init();
					aus(1,3);
					aus(2,1);
					displaystat=true;
				}
			}
			if (displayoff>=120)
			{
				displaystat=false;
				displayoff=0;
				lcd_clear();
				PORTC &= ~(1<<PC2); //Display ausschalten
			}
			
		}
		if (Wan==1 || Wan==2)
		{
			if(debounce(&PIND,2))
			{
				displayoff=0;
				Wan=3;
				lichteinaus(0);
				TCCR0 = (0<<CS01) | (0<<CS00);	//Timer aus
				PORTC &= ~(1<<PC1);				//Port fuer den Lautsprecher aus
			}
		}
		else if (Wan==3)	//Abfrage, damit der Wecker nachdem man ihn in der ersten minute ausgeschaltet hat nicht wieder an geht
		{
			if (WochenTag!=0 && WochenTag!=6)
			{
				if(Stunden==WStunden[0]){
					if(Minuten==WMinuten[0]+1){
						Wan = 0;
					}
				}
			}
			else
			{
				if(Stunden==WStunden[1]){
					if(Minuten==WMinuten[1]+1){
						Wan = 0;
					}
				}
			}
		}
		//Display einschalten bzw. das menue oeffnen, nur wenn die Weckfunktion nicht aktiv ist
		if (debounce(&PIND,PD4) && Wan!=1 && Wan!=2)
		{
			displayoff=0;
			if (!displaystat)
			{
				PORTC |= (1<<PC2); //display einsschalten und neue anzeige starten
				lcd_init();
				aus(1,3);
				aus(2,1);
				displaystat=true;
			}
			else
			{
				menu();
				aus(1,3);	//Ausgabe des Display starten
				aus(2,1);
			}
			
		}
		//Taster fuer die Pos pruefen, um die Lampen einzuschalten, aber nur, wenn der Wecker nicht läuft
		if (debounce(&PIND,PD3) && Wan!=1 && Wan!=2)
		{
			displayoff=0;
			switch (lampenstaerke)
			{
				case 1:
				if ( PORTC & (1<<PC5) )
				{
					lichteinaus(0);	//Licht aus, wenn Licht an
				}
				else
				{
					lichteinaus(1); //nur erste Lampe
				}
					break;
					
				case 2:
				if ( PORTC & (1<<PC5) )
				{
					lichteinaus(0);	//Licht aus, wenn Licht an
				}
				else
				{
					lichteinaus(5); //nur erste Lampe
				}
					break;
					
				case 3:
				if ( PORTC & (1<<PC5) )
				{
					lichteinaus(0);	//Licht aus, wenn Licht an
				}
				else
				{
					lichteinaus(4); //nur erste Lampe
				}
					break;
					
				default:
					break;
			}
			
		}
	}
	
	return 0;
}
