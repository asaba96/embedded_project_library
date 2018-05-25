///////////////////////////////////////
//
// C functions for reading in 
// from the user button
//
// NOTE: button is on GPIO A, Pin 0
//
// You can optionally enable an external 
// interrupt on the button; currently,
// there is a handler defined in 
// animation.c
//
// Created by Andrew Saba
// 5/25/2018
//
///////////////////////////////////////

#include "stm32f429_Discovery_conf.h"

void initButton() {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitTypeDef button;
	
	#ifdef USE_BUTTON_INTERRUPT

	HAL_NVIC_SetPriority(EXTI0_IRQn,2,0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	button.Mode = GPIO_MODE_IT_RISING;

	#else

	button.Mode = GPIO_MODE_INPUT;

	#endif
	
	button.Speed = GPIO_SPEED_MEDIUM;
	button.Pull = GPIO_NOPULL;
	button.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &button);
}

int getButtonLevel() {
	return GPIOA->IDR&1;
}
