/* 
* GOL.cpp
*
* Created: 28.10.2015 16:44:40
* Author: Lüdemann
*/


#include "GOL.h"

// default constructor
GOL::GOL()
{
	delete_playground(0);
	
} //GOL

// default destructor
GOL::~GOL()
{
} //~GOL

void GOL::iteration(){
	uint8_t summe=0;
	delete_playground(2);
	for (uint8_t x = 0; x<GOL_WIDTH; x++)
	{
		for (uint8_t y=0; y<GOL_HIGHT;y++){
			summe = build_sum(x,y);
			if (summe > 3 || summe < 2)
			{
				delete_value(x,y);
			}
			else if (summe == 3 && !playground[x+(y/8)*GOL_WIDTH]&(1<<y&7))
			{
				set_value(x,y);
			}
			else{
				set_value(x,y);
			}
		}
	}
	for (uint16_t i=0; i<GOL_SIZE;i++)
	{
		playground[i]=playground_2[i];
	}
}

void GOL::set_startvalue(){
	//blinker
	delete_playground(0);
	set_value(GOL_WIDTH/2,GOL_HIGHT/2);
	set_value(GOL_WIDTH/2,GOL_HIGHT/2+1);
	set_value(GOL_WIDTH/2,GOL_HIGHT/2-1);
}

void GOL::delete_playground(uint8_t pos){
	if (pos==0)
	{
		for (uint16_t i = 0; i<GOL_SIZE; i++)
		{
			playground[i]=0;
			playground_2[i]=0;
		}
	}
	else if (pos==1)
	{
		for (uint16_t i = 0; i<GOL_SIZE; i++)
		{
			playground[i]=0;
		}
	}
	else if (pos==2)
	{
		for (uint16_t i = 0; i<GOL_SIZE; i++)
		{
			playground_2[i]=0;
		}
	}
}

uint8_t GOL::build_sum(uint8_t x, uint8_t y){
	int8_t temp_x=0, temp_y=0;
	int8_t summe=0;
	
	//x+1 y=0
	temp_x = x+1;
	check_x(&temp_x);
	temp_y = y;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}

	//x-1 y=0
	temp_x = x-1;
	check_x(&temp_x);
	temp_y = y;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	
	//x=0 y+1
	temp_x = x;
	check_x(&temp_x);
	temp_y = y+1;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	
	//x=0 y-1
	temp_x = x;
	check_x(&temp_x);
	temp_y = y-1;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	
	//x-1 y-1
	temp_x = x-1;
	check_x(&temp_x);
	temp_y = y-1;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	
	//x+1 y+1
	temp_x = x+1;
	check_x(&temp_x);
	temp_y = y+1;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	
	//x-1 y+1
	temp_x = x-1;
	check_x(&temp_x);
	temp_y = y+1;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	
	//x+1 y-1
	temp_x = x+1;
	check_x(&temp_x);
	temp_y = y-1;
	check_y(&temp_y);
	summe+=playground[temp_x];
	if (playground[temp_x+(temp_y/8)*GOL_WIDTH]&(1<<temp_y&7))
	{
		summe++;
	}
	return summe;
}

void GOL::check_x(int8_t *x){
	if (*x < 0)
	{
		*x+=GOL_WIDTH;
	}
	else if (*x >=GOL_WIDTH)
	{
		*x -= GOL_WIDTH;
	}
}

void GOL::check_y(int8_t *y){
	if (*y < 0)
	{
		*y += GOL_HIGHT;
	}
	else if (*y >=GOL_HIGHT)
	{
		*y -= GOL_HIGHT;
	}
}

void GOL::set_value(uint8_t x, uint8_t y){
	playground_2[x+(y/8)*GOL_WIDTH] |= (1<<y&7);
}

void GOL::delete_value(uint8_t x, uint8_t y){
	playground_2[x+(y/8)*GOL_WIDTH] &= ~(1<<y&7);
}

