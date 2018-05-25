///////////////////////////////////////
//
// code provided to us to control 
// LED screen pattern
//
///////////////////////////////////////

#include "stm32f429_Discovery_conf.h"

// screen size: 240x320

void LED_SCREEN_Init(){
}

void LED_SCREEN_DispLED(int n, int val){
	int xo,yo,delta,color;
	if((val&1)==0)
		color=SCREEN_COLOR_BLACK;
	else{
		if(n==0)
			color=SCREEN_COLOR_RED;
		else
			if(n==1)
				color=SCREEN_COLOR_YELLOW;
			else
				color=SCREEN_COLOR_GREEN;
	}
	if(n>9)
			return;
	delta=240/10;
	xo=n*delta;
	yo=0;
	SCREEN_DrawFilledRectangle(xo, yo,xo+delta,yo+70,color);
}
