///////////////////////////////////////
//
// C functions for reading in 
// from a rotary encoder and its button
//
// Created by Andrew Saba
// 5/25/2018
//
///////////////////////////////////////

#include <stm32f429_Discovery_conf.h>

#define IDLE 0
#define TURN_LEFT 1
#define TURN_RIGHT 2

int state = IDLE;
int rotEvent = 0;

void encoderInit() {
	__GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef a;
	a.Speed = GPIO_SPEED_MEDIUM;
	a.Mode = GPIO_MODE_INPUT;
	a.Pull = GPIO_NOPULL;
	a.Pin = A_PIN;
	HAL_GPIO_Init(A_GPIO, &a);
	
	GPIO_InitTypeDef b;
	b.Speed = GPIO_SPEED_MEDIUM;
	b.Mode = GPIO_MODE_INPUT;
	b.Pull = GPIO_NOPULL;
	b.Pin = B_PIN;
	HAL_GPIO_Init(B_GPIO, &b);
	
	GPIO_InitTypeDef button;
	button.Speed = GPIO_SPEED_MEDIUM;
	button.Mode = GPIO_MODE_INPUT;
	button.Pull = GPIO_NOPULL;
	button.Pin = BUTTON_ENCODER_PIN;
	HAL_GPIO_Init(BUTTON_ENCODER_GPIO, &button);
}

int getA() {
	return 1-HAL_GPIO_ReadPin(A_GPIO, A_PIN);
}

int getB() {
	return 1-HAL_GPIO_ReadPin(B_GPIO, B_PIN);
}

int getButtonLevelEncoder() {
	return HAL_GPIO_ReadPin(BUTTON_ENCODER_GPIO, BUTTON_ENCODER_PIN);
}

void timerHandler() {
	if (state==IDLE) {
		if (getA()==0 && getB()==0) {
			state = IDLE;
			return;
		} else if (getA()==1) {
			rotEvent = 1;
			state = TURN_RIGHT;
			return;
		} else {
			rotEvent = 2;
			state = TURN_LEFT;
			return;
		}
	} else if (state==TURN_RIGHT || state==TURN_LEFT) {
		if (getA()==1 && getB()==1) {
			state = IDLE;
			return;
		}
		return;
	}
}

int getEvent() {
	int ret = rotEvent;
	rotEvent = 0;
	return ret;
}
