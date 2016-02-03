/* 
* Display.cpp
*
* Created: 02.09.2015 13:43:42
* Author: kevin
*/


#include "Display.h"

// default constructor
Display::Display()
{
	//for (uint8_t i=0;i<1024;i++)
	//{
		//Displayframe[i]=0;
	//}
	InitializeDisplay();
	clearFrame();
} //Display

// default destructor
Display::~Display()
{
} //~Display

void Display::clearFrame(){
	for(uint16_t i=0;i<1024;i++){
		Displayframe[i]=0;
	}
}

void Display::ssd1306_command(uint8_t c){
	i2c.twi_start();
	i2c.twi_write(OLEDADRESSE);
	i2c.twi_write(OLEDBEFEHL);
	i2c.twi_write(c);
	i2c.twi_stop();
}

void Display::InitializeDisplay()
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

void Display::invert(uint8_t inverted) {
	if (inverted) {
		ssd1306_command(SSD1306_INVERTDISPLAY);
	}
	else {
		ssd1306_command(SSD1306_NORMALDISPLAY);
	}
}

void Display::sendFrame(){
	//Cursor an den Anfang setzen
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0x00);
	ssd1306_command(0x7F);

	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0x00);
	ssd1306_command(0x07);

	i2c.twi_start();
	i2c.twi_write(OLEDADRESSE);
	i2c.twi_write(OLEDDATA);
	
	for(uint16_t i=0; i<1024;i++){
		i2c.twi_write(Displayframe[i]);
	}
	i2c.twi_stop();
}

void Display::drawPixel(uint8_t pos_x, uint8_t pos_y, uint8_t erase){
	if(!(pos_x >= SSD1306_WIDTH || pos_y >= SSD1306_HEIGHT)){
		if(!erase){
			Displayframe[pos_x+(pos_y/8)*SSD1306_WIDTH] |= (1 << (pos_y&7));
		}
		else {
			Displayframe[pos_x+(pos_y/8)*SSD1306_WIDTH] &= ~(1 << (pos_y&7));
		}
	}
}

void Display::drawVLine(uint8_t x, uint8_t y, uint8_t length){
	for (uint8_t i = 0; i < length; ++i) {
		drawPixel(x,i+y,0);
	}
}
void Display::drawHLine(uint8_t x, uint8_t y, uint8_t length){
	for (uint8_t i = 0; i < length; ++i) {
		drawPixel(i+x,y,0);
	}
}
void Display::drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill){
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

//eigene Zeichenfunktionen
void Display::draw_line(uint8_t x, uint8_t y, uint8_t length, float phi){
	//phi*=M_PI/180.0;
	uint8_t pos_x=0, pos_y=0;
	for (uint8_t i=0;i<length;i++)
	{
		pos_x=(uint8_t) (-i*cos(phi)+x);
		pos_y=(uint8_t) (i*sin(phi)+y);
		drawPixel(pos_x,pos_y,0);
	}
}

//using basecenterpoint as turningpoint for rotation
//upright triangle has rotationangle of 0
//turning clockwise considered in x transformation
//baseangle=70 is for 1/3 type
void Display::draw_triangle(uint8_t basecenter_x, uint8_t basecenter_y, uint8_t baselength, float baseangle, float rotationangle){
	//correcting rotationangle
	rotationangle*=M_PI/180.0;
	baseangle*=M_PI/180.0;
	//draw baseline
	draw_line(basecenter_x,basecenter_y,baselength/2,rotationangle);
	draw_line(basecenter_x,basecenter_y,baselength/2,rotationangle-M_PI);
	//draw midleline
	uint8_t middlelength=baselength/2*tan(baseangle);
	draw_line(basecenter_x,basecenter_y,middlelength,rotationangle-M_PI_2);
	//draw left arm
	uint8_t lengtharm=baselength/(2*cos(baseangle));
	uint8_t sx = basecenter_x-middlelength*cos(rotationangle-M_PI_2);
	uint8_t sy = basecenter_y+middlelength*sin(rotationangle-M_PI_2);
	draw_line(sx,sy,lengtharm,baseangle+rotationangle);
	//draw right arm
	draw_line(sx,sy,lengtharm,-baseangle+rotationangle-M_PI);
}

void Display::draw_ASCI(uint8_t sym,uint8_t x, uint8_t y){
	if(sym!=' ' && sym>=33){
		sym-=33;
		uint16_t symbol;
		for(uint8_t i = 0; i < 5; i++){
			symbol = pgm_read_byte(&font5x8[sym][i]);
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

void Display::draw_pikachu(){
	for (uint8_t i = 0; i<8;i++)
	{
		for (uint8_t j = 0;j<64;j++)
		{
			Displayframe[i*SSD1306_WIDTH+j]=pgm_read_byte(&Pikachu[i][j]);
		}
	}
}

void Display::draw_glurak(){
	for (uint8_t i = 0; i<8;i++)
	{
		for (uint8_t j = 0;j<64;j++)
		{
			Displayframe[i*SSD1306_WIDTH+j]=pgm_read_byte(&Glurak[i][j]);
		}
	}
}

void Display::draw_number16x16(uint8_t number, uint8_t x, uint8_t y){
	uint16_t symbol;
	if(number>=0 && number<10){
		for(uint8_t i = 0; i < 32;i++){
			symbol = pgm_read_byte(&digits16x16[number][i]);
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

void Display::draw_BIGASCI(uint8_t buch, uint8_t x, uint8_t y){
	uint16_t symbol;
	if(buch!=' ' && buch>='t'){
		buch -= 't';
		for(uint8_t i = 0; i < 16;i++){
			symbol = pgm_read_byte(&font8x16_terminal[buch][i]);
			symbol = symbol << y%8;
			if(i<8){
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
	else{
		for(uint8_t i = 0; i < 7; i++){
			Displayframe[x+(y/8)*SSD1306_WIDTH+i] |= 0;
		}
	}
}

void Display::analog(uint8_t stunde, uint8_t minute, uint8_t sekunde, uint8_t sekanzeige){
	uint8_t r=25;
	uint8_t Px=32-1;
	uint8_t Py=32-1;
	uint8_t x=0,y=0;
	float winkel=0;
	
	if (sekanzeige)
	{
		//Berechnen des Sekundenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(sekunde*6.0)*M_PI/180.0-M_PI/2.0;
			x=(uint8_t) i*cos(winkel)+Px;
			y=(uint8_t) i*sin(winkel)+Py;

			drawPixel(x,y,0);
		}

		r=21;
		//Berechnen des Minutenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(minute*6.0)*M_PI/180.0-M_PI/2.0;
			x=(uint8_t) i*cos(winkel)+Px;
			y=(uint8_t) i*sin(winkel)+Py;

			drawPixel(x,y,0);
		}

		r=13;
		//Berechnen des Stundenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(stunde*30.0+minute/2.0)*M_PI/180.0-M_PI/2.0;
			x= (uint8_t) i*cos(winkel) +Px;
			y= (uint8_t) i*sin(winkel) +Py;

			drawPixel(x,y,0);
		}
	}
	else{
		r=25;
		//Berechnen des Minutenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(minute*6.0)*M_PI/180.0-M_PI/2.0;
			x=(uint8_t) i*cos(winkel)+Px;
			y=(uint8_t) i*sin(winkel)+Py;

			drawPixel(x,y,0);
		}

		r=17;
		//Berechnen des Stundenzeigers
		for(uint8_t i=0;i<r;i++){
			winkel=(stunde*30.0+minute/2.0)*M_PI/180.0-M_PI/2.0;
			x= (uint8_t) i*cos(winkel) +Px;
			y= (uint8_t) i*sin(winkel) +Py;

			drawPixel(x,y,0);
		}
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
