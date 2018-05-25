///////////////////////////////////////
//
// Top level c file for animating 
// dislay with a bar graph pattern
// that lights up sequential bars with 
// changing speed
//
///////////////////////////////////////

#include "stm32f429_Discovery_conf.h"

// define which test to run
// #define TEST1
// #define TEST2
#define TEST3

// test user button by changing 
// LED state based on button level
#ifdef TEST1
void test1() {
	for(;;) {
		int level = getButtonLevel();
		changeLedState(level);
	}
}
#endif

// test animation and LED 
#ifdef TEST2
void test2() {
	int t=0x30000;
	int pattern=1;
	int disp_green = 0;
	for(;;) {
		LED_DISP_Green(disp_green);
		disp_green = 1 - disp_green;
		animation_DisplayPattern(pattern);
		pattern=pattern << 1;
		if (pattern == 0x400)
			pattern=1;
		animation_WaitN(t);
	}
}
#endif 

// test interrupts so don't do anything
#ifdef TEST3
void test3() {
	for(;;) {
	}
}
#endif

void init() {
	SCREEN_Init();
	ledInit();
	initButton();
	encoderInit();
	greenLedInit();
	#ifdef USE_LED_INTERRUPT
	ledTimerInit();
	#endif
}

int main() {
	init();

	#ifdef TEST1
	test1();
	#endif
	
	#ifdef TEST2
	test2();
	#endif
		
	#ifdef TEST3
	test3();
	#endif
	
	return 0;
}
