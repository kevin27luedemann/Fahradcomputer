/*
 * OLED.c
 *
 *  Created on: 07.08.2015
 *      Author: kevin
 */

#include "OLED.h"

//Analoguhr
void analog(uint8_t stunde, uint8_t minute, uint8_t sekunde){
	uint8_t r=25;
	uint8_t Px=32-1;
	uint8_t Py=32-1;
	uint8_t x=0,y=0;
	float winkel=0;

	//Berechnen des Sekundenzeigers
	for(uint8_t i=0;i<r;i++){
		winkel=(sekunde*6.0)*M_PI/180.0-M_PI/2.0;
		x=(uint8_t) i*cos(winkel)+Px;
		y=(uint8_t) i*sin(winkel)+Py;

		drawPixel(x,y,0);
	}

	r=17;
	//Berechnen des Minutenzeigers
	for(uint8_t i=0;i<r;i++){
		winkel=(minute*6.0)*M_PI/180.0-M_PI/2.0;
		x=(uint8_t) i*cos(winkel)+Px;
		y=(uint8_t) i*sin(winkel)+Py;

		drawPixel(x,y,0);
	}

	r=10;
	//Berechnen des Stundenzeigers
	for(uint8_t i=0;i<r;i++){
		winkel=(stunde*30.0)*M_PI/180.0-M_PI/2.0;
		x= (uint8_t) i*cos(winkel) +Px;
		y= (uint8_t) i*sin(winkel) +Py;

		drawPixel(x,y,0);
	}

	//Anzeige der Zahlen
	draw_ASCI('1',32-charsize,0);
	draw_ASCI('2',32,0);
	draw_ASCI('3',64-charsize,4*charhighte-charhighte/2);
	draw_ASCI('6',32-charsize/2,7*charhighte);
	draw_ASCI('9',0,4*charhighte-charhighte/2);

	//anzeige des Rings fuer die Restlichen Stunden Zahlen
	//1
	drawPixel(26*cos((30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//2
	drawPixel(26*cos((2*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((2*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//4
	drawPixel(26*cos((4*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((4*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//5
	drawPixel(26*cos((5*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((5*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//7
	drawPixel(26*cos((7*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((7*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//8
	drawPixel(26*cos((8*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((8*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//10
	drawPixel(26*cos((10*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((10*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
	//11
	drawPixel(26*cos((11*30.0)*M_PI/180.0-M_PI/2.0)+Px,26*sin((11*30.0)*M_PI/180.0-M_PI/2.0)+Px,0);
}


//Drawfunktionen
void drawPixel(uint8_t pos_x, uint8_t pos_y, uint8_t erase){
	if(!(pos_x >= SSD1306_WIDTH || pos_y >= SSD1306_HEIGHT)){
		if(!erase){
			Displayframe[pos_x+(pos_y/8)*SSD1306_WIDTH] |= (1 << (pos_y&7));
		}
		else {
			Displayframe[pos_x+(pos_y/8)*SSD1306_WIDTH] &= ~(1 << (pos_y&7));
		}
	}
}
void drawVLine(uint8_t x, uint8_t y, uint8_t length){
    for (uint8_t i = 0; i < length; ++i) {
        drawPixel(x,i+y,0);
    }
}
void drawHLine(uint8_t x, uint8_t y, uint8_t length){
    for (uint8_t i = 0; i < length; ++i) {
        drawPixel(i+x,y,0);
    }
}
void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill){
    uint8_t length = x2 - x1 + 1;
    uint8_t height = y2 - y1;
	if (!fill) {
        drawHLine(x1,y1,length);
        drawHLine(x1,y2,length);
        drawVLine(x1,y1,height);
        drawVLine(x2,y1,height);
    }
	else {
        for (int x = 0; x < length; ++x) {
            for (int y = 0; y <= height; ++y) {
                drawPixel(x1+x,y+y1,0);
            }
        }
    }
}
void clearFrame(){
	for(uint16_t i=0;i<1024;i++){
		Displayframe[i]=0;
	}
}

//Displayspezifische Funktionen
void sendFrame(){
	//Cursor an den Anfang setzen
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0x00);
	ssd1306_command(0x7F);

	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0x00);
	ssd1306_command(0x07);

	//Schreibe den Buffer
	I2C001_DataType data;

	data.Data1.TDF_Type = I2C_TDF_MStart;
	data.Data1.Data = OLEDADRESSE;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	data.Data1.TDF_Type = I2C_TDF_MTxData;
	data.Data1.Data = OLEDDATA;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	for(uint16_t i=0; i<1024;i++){
		data.Data1.TDF_Type = I2C_TDF_MTxData;
		data.Data1.Data = Displayframe[i];
		while(!I2C001_WriteData(&I2C001_Handle0,&data));
	}

	data.Data1.TDF_Type = I2C_TDF_MStop;
	data.Data1.Data = ubyteFF;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));
}

void invert(uint8_t invert){
	if(invert)
	{
		ssd1306_command(SSD1306_INVERTDISPLAY);
	}
	else
	{
		ssd1306_command(SSD1306_NORMALDISPLAY);
	}
}

void InitializeDisplay()
{
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE

    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                 // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset

    ssd1306_command(SSD1306_SETSTARTLINE);// | 0x0);        // line #0

    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    ssd1306_command(0x14);  // using internal VCC

    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);          // 0x00 horizontal addressing

    ssd1306_command(SSD1306_SEGREMAP | 0x1); // rotate screen 180

    ssd1306_command(SSD1306_COMSCANDEC); // rotate screen 180

    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);

    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0xCF);

    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    ssd1306_command(0xF1);

    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);

    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4

    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

    ssd1306_command(SSD1306_DISPLAYON);                     //switch on OLED
}

void ssd1306_command(uint8_t c){
	I2C001_DataType data;

	data.Data1.TDF_Type = I2C_TDF_MStart;
	data.Data1.Data = OLEDADRESSE;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	data.Data1.TDF_Type = I2C_TDF_MTxData;
	data.Data1.Data = OLEDBEFEHL;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	data.Data1.TDF_Type = I2C_TDF_MTxData;
	data.Data1.Data = c;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	data.Data1.TDF_Type = I2C_TDF_MStop;
	data.Data1.Data = 0;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));
}

void clearDisplay(void){
	//Cursor an den Anfang setzen
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0x00);
	ssd1306_command(0x7F);

	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0x00);
	ssd1306_command(0x07);

	I2C001_DataType data;
	//Senden der Adresse
	data.Data1.TDF_Type = I2C_TDF_MStart;
	data.Data1.Data = OLEDADRESSE;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	data.Data1.TDF_Type = I2C_TDF_MTxData;
	data.Data1.Data = OLEDDATA;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));

	data.Data1.TDF_Type = I2C_TDF_MTxData;
	data.Data1.Data = 0;
	for(uint16_t i=0; i<1024;i++)
	{
		while(!I2C001_WriteData(&I2C001_Handle0,&data));
	}
	//Stopbedingung fuer den Buss
	data.Data1.TDF_Type = I2C_TDF_MStop;
	data.Data1.Data = ubyteFF;
	while(!I2C001_WriteData(&I2C001_Handle0,&data));
}

#include "fonts.h"

void draw_ASCI(uint8_t sym,uint8_t x, uint8_t y){
	if(sym!=' ' && sym>=33){
		sym-=33;
		uint16_t symbol;
		for(uint8_t i = 0; i < 5; i++){
			symbol = font5x8[sym][i];
			symbol = symbol << y%8;
			Displayframe[x+(y/8)*SSD1306_WIDTH+i] |=(uint8_t) (symbol & 0x00FF);
			if(((symbol&0xFF00)>>8)==0){
				Displayframe[x+(y/8+1)*SSD1306_WIDTH+i] |= 0;
			}
			else {
				Displayframe[x+(y/8+1)*SSD1306_WIDTH+i] |= (uint8_t) ((symbol & 0xFF00)>>8);
			}
		}
	}
	else{
		for(uint8_t i = 0; i < 7; i++){
			Displayframe[x+(y/8)*SSD1306_WIDTH+i] |= 0;
		}
	}
}

void draw_number16x16(uint8_t number, uint8_t x, uint8_t y){
	uint16_t symbol;
	if(number>=0 && number<10){
		for(uint8_t i = 0; i < 32;i++){
			symbol = digits16x16[number][i];
			symbol = symbol << y%8;
			if(i<16){
				Displayframe[x+(y/8)*SSD1306_WIDTH+i] |=(uint8_t) (symbol & 0x00FF);
				if(((symbol&0xFF00)>>8)==0){
					Displayframe[x+(y/8+1)*SSD1306_WIDTH+i] |= 0;
				}
				else {
					Displayframe[x+(y/8+1)*SSD1306_WIDTH+i] |= (uint8_t) ((symbol & 0xFF00)>>8);
				}
			}
			else {
				Displayframe[x+(y/8+1)*SSD1306_WIDTH+(i-16)] |= (uint8_t) (symbol & 0x00FF);
				if(((symbol&0xFF00)>>8)==0){
					Displayframe[x+(y/8+2)*SSD1306_WIDTH+(i-16)] |= 0;
				}
				else {
					Displayframe[x+(y/8+2)*SSD1306_WIDTH+(i-16)] |= (uint8_t) ((symbol & 0xFF00)>>8);
				}
			}
		}
	}
}

void draw_number16x24(uint8_t number, uint8_t x, uint8_t y){
	if(number>=0 && number<10){
		for(uint8_t i = 0; i < 48;i++){
			if(i<24){
				Displayframe[x+(y/8)*SSD1306_WIDTH+i] |= digits16x24[number][i];
			}
			else {
				Displayframe[x+(y/8+1)*SSD1306_WIDTH+(i-24)] |= digits16x24[number][i];
			}
		}
	}
}
