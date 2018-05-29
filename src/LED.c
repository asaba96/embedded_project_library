///////////////////////////////////////
//
// C functions for initializing the 
// green LED on the board and for 
// optionally using timer 2 
// to change its state every 250ms
//
// Created by Andrew Saba
// 5/25/2018
//
///////////////////////////////////////

#include <stm32f429_Discovery_conf.h>

TIM_HandleTypeDef myTimer;

int disp_green = 1;

// project required in line 
// arm to enable LED
// TODO fix this to use HAL Library call
void ledInit() {
	/*LDR R0,=0x40023830
	MOV R1, #0x40
	LDR R1, [R0]
	ORR R1, R1, #0x40
	STR R1, [R0]
	bx LR*/
}

// TODO fix this to use HAL library call
void greenLedInit() {
	GPIOG->MODER&=0xF3FFFFFF;
	GPIOG->MODER|=0x04000000;
	GPIOG->OTYPER&=0xFFFFDFFF;
	GPIOG->OSPEEDR&=0xF3FFFFFF;
	GPIOG->ODR&=0xFFFFDFFF;
	GPIOG->ODR|=0x00002000;
}

// if val&1 (so LSB is 1), LED is lit
// else it is not lit
void changeLedState(int val) {
	int * ptr = (int*)0x40021814;
	if((val&1)==1) {
		*ptr = *ptr|(1<<13);
	} else {
		*ptr = *ptr & ~(1<<13);
	}
}

// wait val number of cycles
void wait(int val) {
	while(val>0) {
		val--;
	}
}

// inits timer and interrupts for
// changing LED state
// timer triggers every 250 ms
void ledTimerInit() {	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	HAL_NVIC_SetPriority(TIM2_IRQn,3,0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	__TIM2_CLK_ENABLE();

	myTimer.Instance = TIM2;
	myTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	myTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	myTimer.Init.Period = 5000-1;
	myTimer.Init.Prescaler = 4500-1;
	
	HAL_TIM_Base_Init(&myTimer);
	HAL_TIM_Base_Start_IT(&myTimer);
}

void TIM2_IRQHandler() {
	__HAL_TIM_CLEAR_IT(&myTimer, TIM_FLAG_UPDATE);
	if (disp_green==1){
		disp_green = 0;
	} else {
		disp_green = 1;
	}
	changeLedState(disp_green);
}
