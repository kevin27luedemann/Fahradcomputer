/* 
* GOL.h
* Game of life
*
* Created: 28.10.2015 16:44:40
* Author: Lüdemann
*/


#ifndef __GOL_H__
#define __GOL_H__


//Spielfeldgroesse und Parameter
#define GOL_HIGHT	64
#define GOL_WIDTH	128
#define GOL_SIZE	(GOL_HIGHT/8)*GOL_WIDTH

#include <stdlib.h>
#include <avr/pgmspace.h>

class GOL
{
//variables
public:
uint8_t playground[GOL_SIZE];
uint8_t playground_2[GOL_SIZE];
protected:
private:

//functions
public:
	GOL();
	~GOL();
	void iteration();
	void set_startvalue();
	void delete_playground(uint8_t pos);
protected:
private:
	GOL( const GOL &c );
	GOL& operator=( const GOL &c );
	uint8_t build_sum(uint8_t x, uint8_t y);
	void check_x(int8_t *x);
	void check_y(int8_t *y);
	void set_value(uint8_t x, uint8_t y);
	void delete_value(uint8_t x, uint8_t y);

}; //GOL

#endif //__GOL_H__
