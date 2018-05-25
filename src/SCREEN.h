#ifndef SCREEN_H
#define SCREEN_H

#include "SCREEN_fonts.h"
// version 4 du 15 février 2018

// screen configuration

/**
 * Public enum
 *
 * Select orientation for LCD
 */
typedef enum {
	SCREEN_Orientation_Portrait_1,
	SCREEN_Orientation_Portrait_2,
	SCREEN_Orientation_Landscape_1,
	SCREEN_Orientation_Landscape_2
} SCREEN_Orientation_t;

/**
 * Orientation
 * Used private
 */
typedef enum {
	SCREEN_Landscape,
	SCREEN_Portrait
} SCREEN_Orientation;

typedef struct {
	unsigned short int Width;
	unsigned short int Height;
	SCREEN_Orientation Orientation; // 1 = portrait; 0 = landscape
	unsigned char CurrentLayer;
	unsigned int CurrentLayerOffset;
	unsigned char Layer1Opacity;
	unsigned char Layer2Opacity;
	SCREEN_Orientation_t Orient;
} SCREEN_Config_t;

void SCREEN_Init(void);
void SCREEN_Rotate(SCREEN_Orientation_t);
void SCREEN_SetLayer1(void);
void SCREEN_SetLayer2(void);
void SCREEN_Fill(unsigned int);
void SCREEN_putc(unsigned short int, unsigned short int, char , SCREEN_FontDef_t *, unsigned int, unsigned int);		
void SCREEN_Puts(unsigned short int, unsigned short int, char *, SCREEN_FontDef_t *, unsigned int, unsigned int);
void SCREEN_DrawPixel(unsigned short int, unsigned short int, unsigned int);
void SCREEN_Delay(volatile unsigned int);
void SCREEN_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
void SCREEN_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
void SCREEN_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);



//CSX sur SF-TC240T-9370-T LCD-TFT LCD-RGB LCD-SPI
// PC2 sur CSX ok
#define SCREEN_CS_CLK				RCC_AHB1ENR_GPIOCEN
#define SCREEN_CS_PORT			GPIOC
#define SCREEN_CS_PIN				GPIO_PIN_2

//This pin is used on STM32F429 discovery board
//#define SCREEN_WRX_CLK				RCC_AHB1Periph_GPIOD
//PD13 sur WRX LCD-TFT
#define SCREEN_WRX_CLK			RCC_AHB1ENR_GPIODEN
#define SCREEN_WRX_PORT			GPIOD
#define SCREEN_WRX_PIN			GPIO_PIN_13

//Reset pin
//PD12 sur RDX LCD-TFT
#define SCREEN_RST_CLK			RCC_AHB1ENR_GPIODEN
#define SCREEN_RST_PORT			GPIOD
#define SCREEN_RST_PIN			GPIO_PIN_12

#define SCREEN_RST_SET			HAL_GPIO_WritePin(SCREEN_RST_PORT, SCREEN_RST_PIN,GPIO_PIN_SET)
#define SCREEN_RST_RESET		HAL_GPIO_WritePin(SCREEN_RST_PORT, SCREEN_RST_PIN,GPIO_PIN_RESET)
#define SCREEN_CS_SET				HAL_GPIO_WritePin(SCREEN_CS_PORT, SCREEN_CS_PIN,GPIO_PIN_SET)
#define SCREEN_CS_RESET			HAL_GPIO_WritePin(SCREEN_CS_PORT, SCREEN_CS_PIN,GPIO_PIN_RESET)
#define SCREEN_WRX_SET			HAL_GPIO_WritePin(SCREEN_WRX_PORT, SCREEN_WRX_PIN,GPIO_PIN_SET)
#define SCREEN_WRX_RESET		HAL_GPIO_WritePin(SCREEN_WRX_PORT, SCREEN_WRX_PIN,GPIO_PIN_RESET)

//SPI used
//These SPI pins are used on STM32F429ZIT Discovery board

//----- SPI5 options start -------
#define SCREEN_SPI5_PRESCALER	SPI_BAUDRATEPRESCALER_32
//Specify datasize
#define SCREEN_SPI5_DATASIZE 	SPI_DATASIZE_8BIT
//Specify which bit is first
#define SCREEN_SPI5_FIRSTBIT 	SPI_FIRSTBIT_MSB
//Mode, master or slave
#define SCREEN_SPI5_MASTERSLAVE SPI_MODE_MASTER
//Specify mode of operation, clock polarity and clock phase
#define SCREEN_SPI5_MODE		TM_SPI_Mode_0
//----- SPI5 options end -------

/* Timeout for SDRAM initialization */
#define SDRAM_TIMEOUT			((uint32_t)0xFFFF)


/* SDRAM start address = FMC peripheral start address */
#define SDRAM_START_ADR			(uint32_t)0xD0000000
/* SDRAM max memory width = 8MB */
#define SDRAM_MEMORY_WIDTH		(uint32_t)0x800000
/* Timeout for SDRAM initialization */
#define SDRAM_TIMEOUT			((uint32_t)0xFFFF) 
/**
 * Write 8 bit value to SDRAM
 *
 * Parameters:
 * 	- uint32_t address:
 * 		address from 0x000000 to 0x7FFFFF for 8MBytes
 * 	- uint8_t value:
 * 		value to be saved in SDRAM
 */
#define SCREEN_SDRAM_Write8(address, value)		*(__IO uint8_t *) (SDRAM_START_ADR + address) = value
				
/**
 * Read 8 bit value from SDRAM
 *
 * Parameters:
 * 	- uint32_t address:
 * 		address to read from SDRAM location
 * Return: value stored at desired location
 */
#define SCREEN_SDRAM_Read8(address)				(*(__IO uint8_t *) (SDRAM_START_ADR + address))

//#define ILI9341_SPI 				SPI5
//#define ILI9341_SPI_PINS		TM_SPI_PinsPack_1

//Transparent background, only for chars and strings
#define SCREEN_TRANSPARENT			0x80000000

//Commands
#define ILI9341_RESET					0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA					0x26
#define ILI9341_DISPLAY_OFF		0x28
#define ILI9341_DISPLAY_ON		0x29
#define ILI9341_COLUMN_ADDR		0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM					0x2C
#define ILI9341_MAC						0x36
#define ILI9341_PIXEL_FORMAT	0x3A
#define ILI9341_WDB						0x51
#define ILI9341_WCD						0x53
#define ILI9341_RGB_INTERFACE	0xB0
#define ILI9341_FRC						0xB1
#define ILI9341_BPC						0xB5
#define ILI9341_DFC						0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1					0xC5
#define ILI9341_VCOM2					0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA					0xE8
#define ILI9341_DTCB					0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC						0xF7

//LCD settings
#define SCREEN_WIDTH 				240
#define SCREEN_HEIGHT				320
#define SCREEN_PIXEL				76800

//Starting buffer address in RAM
//Offset for Layer 1 = SDRAM START ADDRESS + FRAME_BUFFER
#define LTDC_FRAME_BUFFER		SDRAM_START_ADR
#define SCREEN_FRAME_BUFFER LTDC_FRAME_BUFFER


//Offset for Layer 2
#define LTDC_FRAME_OFFSET		(uint32_t)SCREEN_PIXEL * 2
#define SCREEN_FRAME_OFFSET		LTDC_FRAME_OFFSET

//Colors
#define SCREEN_COLOR_WHITE			0xFFFF
#define SCREEN_COLOR_BLACK			0x0000
#define SCREEN_COLOR_RED			0xF800
#define SCREEN_COLOR_GREEN			0x07E0
#define SCREEN_COLOR_GREEN2		0xB723
#define SCREEN_COLOR_BLUE			0x001F
#define SCREEN_COLOR_BLUE2			0x051D
#define SCREEN_COLOR_YELLOW		0xFFE0
#define SCREEN_COLOR_ORANGE		0xFBE4
#define SCREEN_COLOR_CYAN			0x07FF
#define SCREEN_COLOR_MAGENTA		0xA254
#define SCREEN_COLOR_GRAY			0x7BEF //1111 0111 1101 1110
#define SCREEN_COLOR_BROWN			0xBBCA

#endif
