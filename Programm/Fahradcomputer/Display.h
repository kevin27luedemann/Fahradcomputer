/* 
* Display.h
*
* Created: 02.09.2015 13:43:42
* Author: kevin
*/


#ifndef __DISPLAY_H__
#define __DISPLAY_H__

//Konstanten
#define SSD1306_WIDTH      128
#define SSD1306_HEIGHT      64
#define SSD1306_SETCONTRAST   0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
#define OLEDADRESSE 0x78
#define OLEDAN 0xAF
#define OLEDAUS 0xAE
#define OLEDON 0xA4
#define OLEDOFF 0xA5
#define OLEDNORMAL 0xA6
#define OLEDINVERSE 0xA7
#define OLEDCONTRAST 0x81
#define OLEDDATA 0x40
#define OLEDBEFEHL 0x00

//Zeichendefinitionen
#define charsize 6
#define charhighte 8
#define numbersmalsize 16
#define numbersmalhight 16
#define numberbigsize 16
#define numberbighight 24

//#include <avr/io.h>
#include <stdlib.h>
#include "I2C.h"
#include <math.h>
#include <avr/pgmspace.h>
#include "fonts.h"

class Display
{
//variables
public:
	uint8_t Displayframe[1024];
protected:
private:
	I2C i2c;

//functions
public:
	Display();
	~Display();
	void clearFrame();
	void ssd1306_command(uint8_t c);
	void InitializeDisplay();
	void invert(uint8_t inverted);
	void sendFrame();
	void drawPixel(uint8_t pos_x, uint8_t pos_y, uint8_t erase);
	void drawVLine(uint8_t x, uint8_t y, uint8_t length);
	void drawHLine(uint8_t x, uint8_t y, uint8_t length);
	void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill);
	void draw_line(uint8_t x, uint8_t y, uint8_t length, float phi);
	void draw_triangle(uint8_t basecenter_x, uint8_t basecenter_y, uint8_t baselength, float baseangle, float rotationangle);
	void draw_ASCI(uint8_t sym,uint8_t x, uint8_t y);
	void draw_number16x16(uint8_t number, uint8_t x, uint8_t y);
	void analog(uint8_t stunde, uint8_t minute, uint8_t sekunde, uint8_t sekanzeige);
protected:
private:
	Display( const Display &c );
	Display& operator=( const Display &c );

}; //Display

#endif //__DISPLAY_H__
