///////////////////////////////////////
//
// For BARGRAPH LED project
// stm32f429_Discovery_conf.h
// version 16/02/2018
//
///////////////////////////////////////

// includes
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "animation.h"
#include "LED_SCREEN.h"
#include "SCREEN.h"
#include "SCREEN_fonts.h"
#include "LED.h"
#include "button.h"
#include "encoder.h"

// GLOBAL MACRO DEFINES
// change these to enable functionality
#define USE_BUTTON_INTERRUPT
#define USE_LED_INTERRUPT

// PIN AND GPIO DEFINITIONS
#define A_PIN GPIO_PIN_11
#define A_GPIO GPIOC
#define B_PIN GPIO_PIN_12
#define B_GPIO GPIOC
#define BUTTON_ENCODER_PIN GPIO_PIN_13
#define BUTTON_ENCODER_GPIO GPIOC
