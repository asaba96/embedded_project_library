///////////////////////////////////////
//
// C functions for animating an
// LED display with a bar graph pattern
//
// Created by Andrew Saba
// 3/28/2018
//
///////////////////////////////////////

#include "stm32f429_Discovery_conf.h"

TIM_HandleTypeDef barTimer;

int count = 0;
int pattern = 1;
int period = 1000; 
int new_period = 1000;
int period_changed = 0;

void animationDisplayPattern(int n) {
	// copy value for naming clarity
	int shifted = n;
	// light all 10 LEDs
	// i is the LED #
	for (int i=0; i<10; i++) {
		// AND shifted value and 1
		// to get LSB
		int val = shifted&1;
		// call function
		LED_SCREEN_DispLED(i,val);
		// shift number to move 
		// next LSB over
		shifted = shifted>>1;
	}
}

void animationWaitN(int n) {
	// execute n operations
	while(n>0)
		n--;
}

// inits timer and interrupts for
// animating display pattern
// timer triggers every 1 ms
void animationTimerInit() {	
	HAL_NVIC_SetPriority(TIM3_IRQn,4,0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	__TIM3_CLK_ENABLE();

	barTimer.Instance = TIM3;
	barTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	barTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	barTimer.Init.Period = 5000-1;
	barTimer.Init.Prescaler = 18-1;
	
	HAL_TIM_Base_Init(&barTimer);
	HAL_TIM_Base_Start_IT(&barTimer);
}

void TIM3_IRQHandler() {
	__HAL_TIM_CLEAR_IT(&barTimer, TIM_FLAG_UPDATE);
	count++;
	if (period_changed) {
		period = new_period;
		period_changed=0;
	}
	if (count%period == 0) { 
		count = 0;
		animationDisplayPattern(pattern);
		pattern=pattern << 1;
		if (pattern == 0x400) {
			pattern=1;
		}	
	}
}

// interrupt handler attached to 
// input of user button
void EXTI0_IRQHandler(void) {
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	if(period_changed==0){
		new_period = new_period/2;
		if (new_period == 0) {
			new_period = 1;
		}
		period_changed = 1;
	}
}
