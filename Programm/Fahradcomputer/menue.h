/*
 * menue.h
 *
 * Created: 05.02.2015 07:21:44
 *  Author: Lüdemann
 */ 


#ifndef MENUE_H_
#define MENUE_H_

uint8_t menu(){
	uint8_t posit = 0;
	while (true)
	{
		switch (posit)
		{
			case 0:
				//Weckzeit anzeigen und einstellen mit PD2
				lcd_clear();
				aus(1,5);
				aus(2,0);
				lcd_string(WeckerWEstring);
				posit = posit + 1;
				while(!debounce(&PIND,PD3))
				{
					//Zeitabfrage mit einbauen
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(4);
						lcd_string(WeckerWEstring);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				break;
			
			case 2:
			//Weckzeit anzeige und einstellungen am Wochenende
				lcd_clear();
				aus(1,7);
				aus(2,0);
				lcd_string(WeckerWOstring);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(5);
						lcd_string(WeckerWOstring);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
			
			case 8:
				//Lampe einstellen
				lcd_clear();
				lcd_string(lampenstaerkestring);
				aus(2,0);
				itoa(lampenstaerke,Buffer,10);
				lcd_string(Buffer);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(1);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
			
			case 5:
				//Zeit einstellen ohnen Sekunden
				lcd_clear();
				aus(1,3);
				aus(2,0);
				lcd_string(Zeitstring);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(0);
						lcd_string(Zeitstring);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
				
			case 6:
				//Sekundenoffset einstellen
				lcd_clear();
				lcd_home();
				lcd_string(sekoffsetstring);
				aus(2,0);
				itoa(sekoffset,Buffer,10);
				lcd_string(Buffer);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(2);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
				
			case 7:
				//Temperaturoffset einstellen
				lcd_clear();
				lcd_home();
				lcd_string(tempoffsetstring);
				aus(2,0);
				itoa(tempoffset,Buffer,10);
				lcd_string(Buffer);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(3);
					}
					if (debounce(&PIND,PD4))
					{
						temperatur();
						return 1;
					}
				}
				temperatur();
				posit++;
				break;
				
			case 4:
				//Weckverzoegerung einstellen
				lcd_clear();
				lcd_home();
				lcd_string(weckverzoegerungstring);
				aus(2,0);
				itoa(weckverzoegerung,Buffer,10);
				lcd_string(Buffer);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						einst(6);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
			case 1:
				//Werktagswecker ausschalten
				lcd_clear();
				lcd_home();
				lcd_string(Werktagsweckeranstring);
				aus(2,0);
				itoa(Werktagsweckeran,Buffer,10);
				lcd_string(Buffer);
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						if (Werktagsweckeran)
						{
							Werktagsweckeran=false;
						}
						else
						{
							Werktagsweckeran=true;
						}
						aus(2,0);
						itoa(Werktagsweckeran,Buffer,10);
						lcd_string(Buffer);
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
			
			case 3:
			//Werktagswecker ausschalten
			lcd_clear();
			lcd_home();
			lcd_string(Wochenendeweckeranstring);
			aus(2,0);
			itoa(Wochenendeweckeran,Buffer,10);
			lcd_string(Buffer);
			while (!debounce(&PIND,PD3))
			{
				if(Sekunden!=sektemp){
					zeit();
				}
				if (debounce(&PIND,PD2))
				{
					if (Wochenendeweckeran)
					{
						Wochenendeweckeran=false;
					}
					else
					{
						Wochenendeweckeran=true;
					}
					aus(2,0);
					itoa(Wochenendeweckeran,Buffer,10);
					lcd_string(Buffer);
				}
				if (debounce(&PIND,PD4))
				{
					return 1;
				}
			}
			posit++;
			break;
				/*
			case 6:
				//Display ausschalten
				lcd_clear();
				lcd_home();
				lcd_string("Display");
				aus(2,0);
				lcd_string("ausschalten?");
				while (!debounce(&PIND,PD3))
				{
					if(Sekunden!=sektemp){
						zeit();
					}
					if (debounce(&PIND,PD2))
					{
						displaystat=false;
						displayoff=0;
						lcd_clear();
						PORTC &= ~(1<<PC2); //Display ausschalten
						return 1;
					}
					if (debounce(&PIND,PD4))
					{
						return 1;
					}
				}
				posit++;
				break;
				*/
			
			default:
				posit=0;
				break;
		}
	}
return 0;
}



#endif /* MENUE_H_ */