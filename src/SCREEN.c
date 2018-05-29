#include <stm32f429_Discovery_conf.h>
// version 4 du 15 f�vrier 2018

// global variable for SCREEN
uint16_t SCREEN_x;
uint16_t SCREEN_y;
SCREEN_Config_t SCREEN_Configs;

// � faire: setColor(col)
// � faire: setPos(x,y) setFont() setForeground setBackground
void SysTick_Handler(){
	HAL_IncTick();
}
void SCREEN_DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
	uint16_t tmp;
	if (x >= SCREEN_Configs.Width) {
		return;
	}
	if (y >= SCREEN_Configs.Height) {
		return;
	}
	if (SCREEN_Configs.Orient == SCREEN_Orientation_Portrait_1) {
		//Portrait1
		*(uint16_t *) (SCREEN_FRAME_BUFFER + SCREEN_Configs.CurrentLayerOffset + 2 * (SCREEN_PIXEL - x - SCREEN_Configs.Width * y)) = color;
	} else if (SCREEN_Configs.Orient == SCREEN_Orientation_Portrait_2) {
		//Portrait2, original
		*(uint16_t *) (SCREEN_FRAME_BUFFER + SCREEN_Configs.CurrentLayerOffset + 2 * (x + SCREEN_Configs.Width * y)) = color;
	} else if (SCREEN_Configs.Orient == SCREEN_Orientation_Landscape_1) {
		//landscape1
		tmp = x;
		x = y;
		y = SCREEN_HEIGHT - 1 - tmp;
		*(uint16_t *) (SCREEN_FRAME_BUFFER + SCREEN_Configs.CurrentLayerOffset + 2 * (x + SCREEN_WIDTH * y)) = color;
	} else {
		//Landscape2
		tmp = y;
		y = x;
		x = SCREEN_WIDTH - 1 - tmp;
		*(uint16_t *) (SCREEN_FRAME_BUFFER + SCREEN_Configs.CurrentLayerOffset + 2 * (x + SCREEN_WIDTH * y)) = color;
	}
	return;
}

void SCREEN_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	
	int16_t dx, dy, sx, sy, err, e2; 
	
	/* Check for overflow */
	if (x0 >= SCREEN_Configs.Width) {
		x0 = SCREEN_Configs.Width - 1;
	}
	if (x1 >= SCREEN_Configs.Width) {
		x1 = SCREEN_Configs.Width - 1;
	}
	if (y0 >= SCREEN_Configs.Height) {
		y0 = SCREEN_Configs.Height - 1;
	}
	if (y1 >= SCREEN_Configs.Height) {
		y1 = SCREEN_Configs.Height - 1;
	}
	
	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	while (1) {
		SCREEN_DrawPixel(x0, y0, color); 
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}

void SCREEN_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	for (; y0 <= y1; y0++) {
		SCREEN_DrawLine(x0, y0, x1, y0, color);
	}
}



void SCREEN_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	SCREEN_DrawLine(x0, y0, x1, y0, color); /* Top */
	SCREEN_DrawLine(x0, y0, x0, y1, color);	/* Left */
	SCREEN_DrawLine(x1, y0, x1, y1, color);	/* Right */
	SCREEN_DrawLine(x0, y1, x1, y1, color);	/* Bottom */
}

void SCREEN_putc(uint16_t x, uint16_t y, char c, SCREEN_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint32_t i, b, j;
	/* Set coordinates */
	SCREEN_x = x;
	SCREEN_y = y;
	if ((SCREEN_x + font->FontWidth) > SCREEN_Configs.Width) {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		SCREEN_y += font->FontHeight;
		SCREEN_x = 0;
	}
	for (i = 0; i < font->FontHeight; i++) {
		b = font->data[(c - 32) * font->FontHeight + i];
		for (j = 0; j < font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				SCREEN_DrawPixel(SCREEN_x + j, (SCREEN_y + i), foreground);
			} else if ((background & SCREEN_TRANSPARENT) == 0) {
				SCREEN_DrawPixel(SCREEN_x + j, (SCREEN_y + i), background);
			}
		}
	}
	/* Go to new X location */
	SCREEN_x += font->FontWidth;
}
void SCREEN_Rotate(SCREEN_Orientation_t orientation) {
	SCREEN_Configs.Orient = orientation;
	if (orientation == SCREEN_Orientation_Portrait_1 || orientation == SCREEN_Orientation_Portrait_2) {
		SCREEN_Configs.Width = SCREEN_WIDTH;
		SCREEN_Configs.Height = SCREEN_HEIGHT;
		SCREEN_Configs.Orientation = SCREEN_Portrait;
	} else {
		SCREEN_Configs.Width = SCREEN_HEIGHT;
		SCREEN_Configs.Height = SCREEN_WIDTH;
		SCREEN_Configs.Orientation = SCREEN_Landscape;
	}
}
void SCREEN_Puts(uint16_t x, uint16_t y, char *str, SCREEN_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint16_t startX = x;
	
	/* Set X and Y coordinates */
	SCREEN_x = x;
	SCREEN_y = y;
	
	while (*str) {
		/* New line */
		if (*str == '\n') {
			SCREEN_y += font->FontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				SCREEN_x = 0;
				str++;
			} else {
				SCREEN_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}
		
		/* Put character */
		SCREEN_putc(SCREEN_x, SCREEN_y, *str++, font, foreground, background);
	}
}

/**********************************************************/
/*     INITIALIZATION of the frequencies for the SCREEN   */
/**********************************************************/
void SCREEN_SystemClock_init(){
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	// HSE = 8MHz : STM32F429 Discovery
	// Goal: SYSCLK 180 MHz
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState =RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks
dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
	RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	//AHB 180 MHz
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	//APB1 45 MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	// init de la sysclk
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
/**********************************************************/
/*        INITIALIZATION of the Pins                      */
/**********************************************************/
void SCREEN_Pins_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//Enable clock
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();
	
	//GPIO_InitStruct.Speed = GPIO_Speed_100MHz;
	// 2  25     50   100 MHz
	//LOW MEDIUM FAST HIGH
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//WRX pin on PD13
	GPIO_InitStruct.Pin = SCREEN_WRX_PIN;
	HAL_GPIO_Init(SCREEN_WRX_PORT, &GPIO_InitStruct);
	//SPI CS pin on 
	GPIO_InitStruct.Pin = SCREEN_CS_PIN;
	HAL_GPIO_Init(SCREEN_CS_PORT, &GPIO_InitStruct);	
	//Reset pin
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = SCREEN_RST_PIN;
	HAL_GPIO_Init(SCREEN_RST_PORT, &GPIO_InitStruct);
	//Common settings
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	//GPIOA					     	Blue5					VSYNC		 		Green2		  	Red4					Red5
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//GPIOA 							Red3	PB0			Red6 PB1 in AF 9 GPIO_AF9_LTDC	 
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	//GPIOB					   			Blue6		  		Blue7		   Green4		 			Green5
	GPIO_InitStruct.Pin =  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//GPIOC					   						HSYNC				Green6		 		Red2
	//GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;
	//GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_InitStruct.Pin =  GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 ;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	//GPIOD					   		Green7			Blue2
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_6;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	//GPIOF					   Enable
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	//GPIOG					   			Red7				DOTCLK		 		Blue3		 
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 |  GPIO_PIN_11;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	//GPIOG					   		 Green3			  Blue4
	GPIO_InitStruct.Pin =  GPIO_PIN_10 |   GPIO_PIN_12;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);	
}

/**********************************************************/
/*         INITIALIZATION of the SPI5                     */
/**********************************************************/
void SCREEN_SPI5_Init() {
	// pinspack 1 SPI5_Mode
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_HandleTypeDef SPI_HandleStruct;
	//Common settings for all SPI5 pins
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	
	//Enable clock
	__GPIOF_CLK_ENABLE();
	//Pinspack nr. 1        SCK          MISO         MOSI
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	__SPI5_CLK_ENABLE();
	SPI_HandleStruct.Instance=SPI5;
	HAL_SPI_DeInit(&SPI_HandleStruct);
	SPI_HandleStruct.Init.BaudRatePrescaler=SCREEN_SPI5_PRESCALER;
	SPI_HandleStruct.Init.DataSize=SCREEN_SPI5_DATASIZE;
	SPI_HandleStruct.Init.Direction=SPI_DIRECTION_2LINES;
	SPI_HandleStruct.Init.FirstBit=SCREEN_SPI5_FIRSTBIT;
	SPI_HandleStruct.Init.Mode=SCREEN_SPI5_MASTERSLAVE;
	SPI_HandleStruct.Init.CLKPolarity=SPI_POLARITY_LOW;
	SPI_HandleStruct.Init.CLKPhase=SPI_PHASE_1EDGE;
	SPI_HandleStruct.Init.NSS=SPI_NSS_SOFT;
	SPI_HandleStruct.Init.TIMode=SPI_TIMODE_DISABLED;
	SPI_HandleStruct.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLED;
	SPI_HandleStruct.Init.CRCPolynomial=0;
	//SPI_Cmd(SPI5, DISABLE);
	SPI_HandleStruct.Instance->CR1&=~(1<<6);
	//SPI_DeInit(SPI5);
	HAL_SPI_DeInit(&SPI_HandleStruct);
	HAL_SPI_Init(&SPI_HandleStruct);
	//SPI_Cmd(SPI5, ENABLE);
	SPI_HandleStruct.Instance->CR1|=(1<<6);
}
/**********************************************************/
/*         INITIALIZATION of the pins of the SDRAM        */
/**********************************************************/
void SCREEN_SDRAM_InitPins(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	//Common settings
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	//GPIOB pins for FMC
	__GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	//                      SDCKE1       SDNE1
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//GPIOC pins for FMC
	__GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	//                      SDNWE
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	//GPIOD pins for FMC
	__GPIOD_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	//                      D2           D3           D13          D14          D15           D0            D13  
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	//GPIOE pins for FMC
	__GPIOE_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	//                      NBL0         NBL1         D4           D5           D6           D7            D8            D9            D10           D11           D12            
	GPIO_InitStruct.Pin =GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	//GPIOF pins for FMC
	__GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	//                      A0           A1           A2           A3           A4           A5           SDNRAS        A6            A7            A8            A9
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	//GPIOG pins for FMC
	__GPIOG_CLK_ENABLE();
	GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
	//                      A10          A11          BA0          BA1          SDCLK        SDNCAS
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/**********************************************************/
/*         INITIALIZATION of the SDRAM                    */
/**********************************************************/
uint8_t SCREEN_SDRAM_Init(void) {
	SDRAM_HandleTypeDef hsdram;
	FMC_SDRAM_TimingTypeDef timing;
	FMC_SDRAM_CommandTypeDef command;
	int ok=0;

	hsdram.Instance=FMC_Bank5_6;// FMC_R_BASE=0xA000 0000
	//FMC_Bank5_6_R_BASE 0xA000 0000+0x140 adresse de FCM_SDCR1
	//hsdram.Instance->SDCR[0]=1; 0xA000 0000+0x140
	/* Initialize FMC pins */
	SCREEN_SDRAM_InitPins();
	/* Enable FMC clock */
	__FMC_CLK_ENABLE();	
	/* FMC SDRAM device initialization sequence --------------------------------*/ 
	/* Step 1 ----------------------------------------------------*/ 
	/* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
	/* TMRD: 2 Clock cycles */
	/* 1 clock cycle = 1 / 90MHz = 11.1ns */
	timing.LoadToActiveDelay=2;
	/* TXSR: min=70ns (7x11.10ns) */
	timing.ExitSelfRefreshDelay=7;
	/* TRAS: min=42ns (4x11.10ns) max=120k (ns) */
	timing.SelfRefreshTime=4;
	/* TRC:  min=70 (7x11.10ns) */        
	timing.RowCycleDelay=7;
	/* TWR:  min=1+ 7ns (1+1x11.10ns) */
	timing.WriteRecoveryTime=2;
	/* TRP:  20ns => 2x11.10ns */
	timing.RPDelay=2;
	/* TRCD: 20ns => 2x11.10ns */
	timing.RCDDelay=2;	
	/* FMC SDRAM control configuration */
	hsdram.Init.SDBank=FMC_SDRAM_BANK2;//1
	/* Row addressing: [7:0] */
	hsdram.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_8;
	/* Column addressing: [11:0] */
	hsdram.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_12;//4 dans FMC_SDCR2 cad en 0x140+4 bit NR(0)
	hsdram.Init.MemoryDataWidth=FMC_NORSRAM_MEM_BUS_WIDTH_16;
	hsdram.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_2;
	/* CL: Cas Latency = 3 clock cycles */
	hsdram.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_3;
	hsdram.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	hsdram.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;//0x800
	hsdram.Init.ReadBurst=FMC_SDRAM_RBURST_DISABLE;
	hsdram.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_1;
	/* FMC SDRAM bank initialization */
	HAL_SDRAM_Init(&hsdram,&timing);	
	/* Configure a clock configuration enable command */
	command.CommandMode=FMC_SDRAM_CMD_CLK_ENABLE;
	command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	command.AutoRefreshNumber=1;
	command.ModeRegisterDefinition=0;	
	/* Wait until the SDRAM controller is ready */ 
	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram,&command,SDRAM_TIMEOUT);
	/* Configure a PALL (precharge all) command */ 
	command.CommandMode=FMC_SDRAM_CMD_PALL;
	command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	command.AutoRefreshNumber=1;
	command.ModeRegisterDefinition=0;	
	/* Wait until the SDRAM controller is ready */  
	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram,&command,SDRAM_TIMEOUT);
	/* Configure a Auto-Refresh command */ 
	command.CommandMode=FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	command.AutoRefreshNumber=8;
	command.ModeRegisterDefinition=0;	
	/* Wait until the SDRAM controller is ready */
	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram,&command,SDRAM_TIMEOUT);	
	/* Configure a load Mode register command */
	command.CommandMode=FMC_SDRAM_CMD_LOAD_MODE;
	command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	command.AutoRefreshNumber=1;
	command.ModeRegisterDefinition=0x0231;	
	/* Wait until the SDRAM controller is ready */
	/* Send the command */
	HAL_SDRAM_SendCommand(&hsdram,&command,SDRAM_TIMEOUT);
	/* Set the refresh rate counter */
	/* (7.81 us x Freq) - 20 = (7.81 * 90MHz) - 20 = 683 */
	/* Set the device refresh counter */
	//FMC_SetRefreshCount(680);
	// dans FMC_SDRTR count de bit 1 � 13
	// val de 0 � 8191 
	//HAL_SDRAM_SetAutoRefreshNumber(&hsdram,);
	//ne convient pas car Device->SDCMR |= (AutoRefreshNumber << 5); 
	//on veut hsdram.Instance->SDRTR � �crire
	HAL_SDRAM_ProgramRefreshRate(&hsdram,680);	
	/* Wait until the SDRAM controller is ready */ 
	/* Check if everything goes right */
	/* Write 0x10 at location 0x50 and check if result is the same on read operation */
	SCREEN_SDRAM_Write8(0x50, 0x10);
	if (SCREEN_SDRAM_Read8(0x50) == 0x10) {
		//Initialized OK
		//return 1;
		ok++;
	}
	SCREEN_SDRAM_Write8(0x51, 0xEF);
	if (SCREEN_SDRAM_Read8(0x51) == 0xEF) {
		//Initialized OK
		//return 1;
		ok++;
	}
	if(ok==2)
		ok=1;
	else
		ok=0;
		//Not OK
	return ok;
}
/**********************************************************/
/*         INITIALIZATION of the LCD SCREEN               */
/**********************************************************/
uint8_t SCREEN_SPI_Send(uint8_t data) {
	/* Fill output buffer with data */
	SPI5->DR = data;
	/* Wait for transmission to complete */
	//while (!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));
	while ((SPI5->SR & SPI_FLAG_TXE)!=SPI_FLAG_TXE);
	/* Wait for received data to complete */
	//while (!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
	// bizarre tout de meme! c est une transmission
	while ((SPI5->SR & SPI_FLAG_RXNE)!=SPI_FLAG_RXNE);
	/* Wait for SPI to be ready */
	//while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY));
	while ((SPI5->SR & SPI_FLAG_BSY)==SPI_FLAG_BSY);
	/* Return data from buffer */
	return SPI5->DR;
}

void SCREEN_LCD_SendCommand(uint8_t data) {
	SCREEN_WRX_RESET;
	SCREEN_CS_RESET;
	SCREEN_SPI_Send(data);
	SCREEN_CS_SET;
}

void SCREEN_LCD_SendData(uint8_t data) {
	SCREEN_WRX_SET;
	SCREEN_CS_RESET;
	SCREEN_SPI_Send(data);
	SCREEN_CS_SET;
}

void SCREEN_Delay(volatile unsigned int delay) {
	for (; delay != 0; delay--); 
}

void SCREEN_LCD_Init(void) {
	//doc ILI9341
	SCREEN_LCD_SendCommand(0xCA);
	SCREEN_LCD_SendData(0xC3);
	SCREEN_LCD_SendData(0x08);
	SCREEN_LCD_SendData(0x50);
	SCREEN_LCD_SendCommand(ILI9341_POWERB);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0xC1);
	SCREEN_LCD_SendData(0x30);
	SCREEN_LCD_SendCommand(ILI9341_POWER_SEQ);
	SCREEN_LCD_SendData(0x64);
	SCREEN_LCD_SendData(0x03);
	SCREEN_LCD_SendData(0x12);
	SCREEN_LCD_SendData(0x81);
	SCREEN_LCD_SendCommand(ILI9341_DTCA);
	SCREEN_LCD_SendData(0x85);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x78);
	SCREEN_LCD_SendCommand(ILI9341_POWERA);
	SCREEN_LCD_SendData(0x39);
	SCREEN_LCD_SendData(0x2C);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x34);
	SCREEN_LCD_SendData(0x02);
	SCREEN_LCD_SendCommand(ILI9341_PRC);
	SCREEN_LCD_SendData(0x20);
	SCREEN_LCD_SendCommand(ILI9341_DTCB);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendCommand(ILI9341_FRC);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x1B);
	SCREEN_LCD_SendCommand(ILI9341_DFC);
	SCREEN_LCD_SendData(0x0A);
	SCREEN_LCD_SendData(0xA2);
	SCREEN_LCD_SendCommand(ILI9341_POWER1);
	SCREEN_LCD_SendData(0x10);
	SCREEN_LCD_SendCommand(ILI9341_POWER2);
	SCREEN_LCD_SendData(0x10);
	SCREEN_LCD_SendCommand(ILI9341_VCOM1);
	SCREEN_LCD_SendData(0x45);
	SCREEN_LCD_SendData(0x15);
	SCREEN_LCD_SendCommand(ILI9341_VCOM2);
	SCREEN_LCD_SendData(0x90);
	SCREEN_LCD_SendCommand(ILI9341_MAC);
	SCREEN_LCD_SendData(0xC8);
	SCREEN_LCD_SendCommand(ILI9341_3GAMMA_EN);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendCommand(ILI9341_RGB_INTERFACE);
	SCREEN_LCD_SendData(0xC2);
	SCREEN_LCD_SendCommand(ILI9341_DFC);
	SCREEN_LCD_SendData(0x0A);
	SCREEN_LCD_SendData(0xA7);
	SCREEN_LCD_SendData(0x27);
	SCREEN_LCD_SendData(0x04);

	SCREEN_LCD_SendCommand(ILI9341_COLUMN_ADDR);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0xEF);

	SCREEN_LCD_SendCommand(ILI9341_PAGE_ADDR);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x01);
	SCREEN_LCD_SendData(0x3F);
	SCREEN_LCD_SendCommand(ILI9341_INTERFACE);
	SCREEN_LCD_SendData(0x01);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x06);

	SCREEN_LCD_SendCommand(ILI9341_GRAM);
	SCREEN_Delay(1000000);

	SCREEN_LCD_SendCommand(ILI9341_GAMMA);
	SCREEN_LCD_SendData(0x01);

	SCREEN_LCD_SendCommand(ILI9341_PGAMMA);
	SCREEN_LCD_SendData(0x0F);
	SCREEN_LCD_SendData(0x29);
	SCREEN_LCD_SendData(0x24);
	SCREEN_LCD_SendData(0x0C);
	SCREEN_LCD_SendData(0x0E);
	SCREEN_LCD_SendData(0x09);
	SCREEN_LCD_SendData(0x4E);
	SCREEN_LCD_SendData(0x78);
	SCREEN_LCD_SendData(0x3C);
	SCREEN_LCD_SendData(0x09);
	SCREEN_LCD_SendData(0x13);
	SCREEN_LCD_SendData(0x05);
	SCREEN_LCD_SendData(0x17);
	SCREEN_LCD_SendData(0x11);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendCommand(ILI9341_NGAMMA);
	SCREEN_LCD_SendData(0x00);
	SCREEN_LCD_SendData(0x16);
	SCREEN_LCD_SendData(0x1B);
	SCREEN_LCD_SendData(0x04);
	SCREEN_LCD_SendData(0x11);
	SCREEN_LCD_SendData(0x07);
	SCREEN_LCD_SendData(0x31);
	SCREEN_LCD_SendData(0x33);
	SCREEN_LCD_SendData(0x42);
	SCREEN_LCD_SendData(0x05);
	SCREEN_LCD_SendData(0x0C);
	SCREEN_LCD_SendData(0x0A);
	SCREEN_LCD_SendData(0x28);
	SCREEN_LCD_SendData(0x2F);
	SCREEN_LCD_SendData(0x0F);

	SCREEN_LCD_SendCommand(ILI9341_SLEEP_OUT);
	SCREEN_Delay(1000000);
	SCREEN_LCD_SendCommand(ILI9341_DISPLAY_ON);

	SCREEN_LCD_SendCommand(ILI9341_GRAM);
}

/**********************************************************/
/*        INITIALIZATION of the LTDC module               */
/**********************************************************/

void SCREEN_LTDC_Init(void) {
	LTDC_HandleTypeDef LTDC_HandleInitStructure;
	RCC_PeriphCLKInitTypeDef  p_PeriphClkInit;
//	int *pL1CFBLR,*pL2CFBLR;
	/* Enable the LTDC Clock */
	__LTDC_CLK_ENABLE();
	/* Enable the DMA2D Clock */
	__DMA2D_CLK_ENABLE();	

	/* Configure PLLSAI prescalers for LCD */
	/* Enable Pixel Clock */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	// ici HSE=8 MHz PLL_M=8 
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
	// donc PLLSAI_N=192
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/4 = 96 Mhz */
	// ca devrait etre 48 MHz car 192/4=48
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 96/4 = 24 Mhz */
	// il y a un diviseur apr�s appel� DIV 
	// est ce que DIV=2 => frequ obtenue = 48/2=24MHz?
	// ou alors c'est que l'init est en fait � 12 MHz et non � 24 comme annonc�
	
	//RCC_PLLSAIConfig(192, 7, 4);
	//RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);
	/* Enable PLLSAI Clock */
  //  RCC_PLLSAICmd(ENABLE); fait dans init:
	// __HAL_RCC_PLLSAI_ENABLE(); vu dans HAL_RCCEx_PeriphCLKConfig
  /* Wait for PLLSAI activation */
	//while (RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET);
	//p_PeriphClkInit.PeriphClockSelection=RCC_PERIPHCLK_SAI_PLLSAI;
	p_PeriphClkInit.PeriphClockSelection=RCC_PERIPHCLK_LTDC;
	p_PeriphClkInit.PLLSAI.PLLSAIN=192;
	p_PeriphClkInit.PLLSAI.PLLSAIQ=7;
	p_PeriphClkInit.PLLSAI.PLLSAIR=4;//4
	p_PeriphClkInit.PLLSAIDivR=RCC_PLLSAIDIVR_4;//RCC_PLLSAIDIVR_4
	HAL_RCCEx_PeriphCLKConfig(&p_PeriphClkInit);
	
	LTDC_HandleInitStructure.Instance=LTDC;
	//__HAL_LTDC_LAYER_ENABLE(&LTDC_HandleInitStructure,0);
	//__HAL_LTDC_LAYER_ENABLE(&LTDC_HandleInitStructure,1);
	HAL_LTDC_DeInit(&LTDC_HandleInitStructure);
	
	/* Polarity configuration */
	/* Initialize the horizontal synchronization polarity as active low */

	LTDC_HandleInitStructure.Init.HSPolarity = LTDC_HSPOLARITY_AL;     
	/* Initialize the vertical synchronization polarity as active low */  
	LTDC_HandleInitStructure.Init.VSPolarity = LTDC_VSPOLARITY_AL;     
	/* Initialize the data enable polarity as active low */
	LTDC_HandleInitStructure.Init.DEPolarity = LTDC_DEPOLARITY_AL;     
	/* Initialize the pixel clock polarity as input pixel clock */ 
	LTDC_HandleInitStructure.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

	/* Configure R,G,B component values for LCD background color */                   
	//LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
	//LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
	//LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;
	LTDC_HandleInitStructure.Init.Backcolor.Red=0;
	LTDC_HandleInitStructure.Init.Backcolor.Green=0;
	LTDC_HandleInitStructure.Init.Backcolor.Blue=0;
  /* Timing configuration */  
  /* Configure horizontal synchronization width */     
  LTDC_HandleInitStructure.Init.HorizontalSync = 9;
  /* Configure vertical synchronization height */
  LTDC_HandleInitStructure.Init.VerticalSync = 1;
  /* Configure accumulated horizontal back porch */
  LTDC_HandleInitStructure.Init.AccumulatedHBP = 29; 
  /* Configure accumulated vertical back porch */
  LTDC_HandleInitStructure.Init.AccumulatedVBP = 3;  
  /* Configure accumulated active width */  
  LTDC_HandleInitStructure.Init.AccumulatedActiveW = 269;
  /* Configure accumulated active height */
  LTDC_HandleInitStructure.Init.AccumulatedActiveH = 323;
  /* Configure total width */
  LTDC_HandleInitStructure.Init.TotalWidth = 279; 
  /* Configure total height */
  LTDC_HandleInitStructure.Init.TotalHeigh = 327;
	
	// initialisation � ajouter?
	//initLayers en particulier?
		/* 	Windowing configuration */
	/* 	Horizontal start = horizontal synchronization + Horizontal back porch = 43 
		Vertical start   = vertical synchronization + vertical back porch     = 12
		Horizontal stop = Horizontal start + LCD_PIXEL_WIDTH - 1 
		Vertical stop   = Vertical start + LCD_PIXEL_HEIGHT - 1        
	*/      
	//LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
	LTDC_HandleInitStructure.LayerCfg[0].WindowX0=0;//30;
	//LTDC_Layer_InitStruct.LTDC_HorizontalStop = 269;
	LTDC_HandleInitStructure.LayerCfg[0].WindowX1=269-(30-1);
	//LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
	LTDC_HandleInitStructure.LayerCfg[0].WindowY0=0;//4;
	//LTDC_Layer_InitStruct.LTDC_VerticalStop = 323;
	LTDC_HandleInitStructure.LayerCfg[0].WindowY1=323-(4-1);
	/* Pixel Format configuration*/
  //LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	LTDC_HandleInitStructure.LayerCfg[0].PixelFormat=LTDC_PIXEL_FORMAT_RGB565;
	/* Alpha constant (255 totally opaque) */
  //LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
	LTDC_HandleInitStructure.LayerCfg[0].Alpha=255; //constant alpha for blending
	/* Default Color configuration (configure A,R,G,B component values) */          
  //LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  //LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  //LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  //LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	LTDC_HandleInitStructure.LayerCfg[0].Backcolor.Blue=0;
	LTDC_HandleInitStructure.LayerCfg[0].Backcolor.Green=0;
	LTDC_HandleInitStructure.LayerCfg[0].Backcolor.Red=0;
	LTDC_HandleInitStructure.LayerCfg[0].Alpha0=0;
	
	/* Configure blending factors */       
  //LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  //LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
	LTDC_HandleInitStructure.LayerCfg[0].BlendingFactor1=LTDC_BLENDING_FACTOR1_CA;
	LTDC_HandleInitStructure.LayerCfg[0].BlendingFactor2=LTDC_BLENDING_FACTOR2_CA;
	
	/* the length of one line of pixels in bytes + 3 then :
    Line Lenth = Active high width x number of bytes per pixel + 3 
    Active high width         = LCD_PIXEL_WIDTH 
    number of bytes per pixel = 2    (pixel_format : RGB565) 
    */
	//LTDC_Layer_InitStruct.LTDC_CFBLineLength = 240 * 2 + 3;
	LTDC_HandleInitStructure.LayerCfg[0].ImageWidth=240;//240 * 2 + 3;
	
		/* the pitch is the increment from the start of one line of pixels to the 
    start of the next line in bytes, then :
    Pitch = Active high width x number of bytes per pixel     
    */ 
	//LTDC_Layer_InitStruct.LTDC_CFBPitch = 240 * 2;
	//LTDC_L0CFBLR&=0xFFFF;
	//LTDC_L0CFBLR|=(240*2)<<16;
	//LTDC address = 0x4000 0000+0x0001 0000+0x0000 6800= 0x4001 6800
	// offset pour L1CFBLR = 0xB0 + 0x80.(1-1)=0xB0
	// L1CFBLR en 0x4001 68B0
	
	//pL1CFBLR=(int*) (0x400168B0);
	//(*pL1CFBLR)&=0xFFFF;
	//(*pL1CFBLR)|=((240*2)<<16);
	
	/* Configure the number of lines */ 
	//LTDC_Layer_InitStruct.LTDC_CFBLineNumber = 320;
	LTDC_HandleInitStructure.LayerCfg[0].ImageHeight=320;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */   
	//LTDC_Layer_InitStruct.LTDC_CFBStartAdress = ILI9341_FRAME_BUFFER;
	LTDC_HandleInitStructure.LayerCfg[0].FBStartAdress=LTDC_FRAME_BUFFER;
	/* Initialize Layer 1 */
	//LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

   /* Configure Layer2 */
	 	//LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
	LTDC_HandleInitStructure.LayerCfg[1].WindowX0=0;//30;
	//LTDC_Layer_InitStruct.LTDC_HorizontalStop = 269;
	LTDC_HandleInitStructure.LayerCfg[1].WindowX1=269-(30-1);
	//LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
	LTDC_HandleInitStructure.LayerCfg[1].WindowY0=0;//4;
	//LTDC_Layer_InitStruct.LTDC_VerticalStop = 323;
	LTDC_HandleInitStructure.LayerCfg[1].WindowY1=323-(4-1);
	/* Pixel Format configuration*/
  //LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	LTDC_HandleInitStructure.LayerCfg[1].PixelFormat=LTDC_PIXEL_FORMAT_RGB565;
	/* Alpha constant (255 totally opaque) */
  //LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
	LTDC_HandleInitStructure.LayerCfg[1].Alpha=255; //constant alpha for blending
	/* Default Color configuration (configure A,R,G,B component values) */          
  //LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  //LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  //LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  //LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	LTDC_HandleInitStructure.LayerCfg[1].Backcolor.Blue=0;
	LTDC_HandleInitStructure.LayerCfg[1].Backcolor.Green=0;
	LTDC_HandleInitStructure.LayerCfg[1].Backcolor.Red=0;
	LTDC_HandleInitStructure.LayerCfg[1].Alpha0=0;
	

	LTDC_HandleInitStructure.LayerCfg[1].ImageWidth=240;//240 * 2 + 3;
	
		/* the pitch is the increment from the start of one line of pixels to the 
    start of the next line in bytes, then :
    Pitch = Active high width x number of bytes per pixel     
    */ 
	//LTDC_Layer_InitStruct.LTDC_CFBPitch = 240 * 2;
	//LTDC_HandleInitStructure.LayerCfg[0].ImageHeight=240 * 2;
	//LTDC_HandleInitStructure.Instance-> 
	//LTDC_L0CFBLR&=0xFFFF;
	//LTDC_L0CFBLR|=(240*2)<<16;
	//LTDC address = 0x4000 0000+0x0001 0000+0x0000 6800= 0x4001 6800
	// offset pour L2CFBLR = 0xB0 + 0x80.(2-1)=0x130
	// L1CFBLR en 0x4001 6930
	//pL2CFBLR=(int*) (0x40016930);
	//(*pL2CFBLR)&=0xFFFF;
	//(*pL2CFBLR)|=((240*2)<<16);
	
	LTDC_HandleInitStructure.LayerCfg[1].ImageHeight=320;
	
  /* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */  
	//LTDC_Layer_InitStruct.LTDC_CFBStartAdress = ILI9341_FRAME_BUFFER + ILI9341_FRAME_OFFSET;
	LTDC_HandleInitStructure.LayerCfg[1].FBStartAdress=LTDC_FRAME_BUFFER+ LTDC_FRAME_OFFSET;
	/* Configure blending factors */ 
	//LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
	//LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;	     
	LTDC_HandleInitStructure.LayerCfg[1].BlendingFactor1=LTDC_BLENDING_FACTOR1_PAxCA;
	LTDC_HandleInitStructure.LayerCfg[1].BlendingFactor2=LTDC_BLENDING_FACTOR2_PAxCA;
	/* Initialize Layer 2 */
	
	HAL_LTDC_Init(&LTDC_HandleInitStructure);
	
	//LTDC_ReloadConfig(LTDC_IMReload);
	__HAL_LTDC_RELOAD_CONFIG(&LTDC_HandleInitStructure);
	
	/* Enable foreground & background Layers */
	//LTDC_LayerCmd(LTDC_Layer1, ENABLE);
	//LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	//mis avant en ENABLE: __HAL_LTDC_LAYER_ENABLE(&LTDC_HandleInitStructure,0);

	__HAL_LTDC_LAYER_ENABLE(&LTDC_HandleInitStructure,0);
	__HAL_LTDC_LAYER_ENABLE(&LTDC_HandleInitStructure,1);
	
	/* Reload immediate */
	//LTDC_ReloadConfig(LTDC_IMReload);
	__HAL_LTDC_RELOAD_CONFIG(&LTDC_HandleInitStructure);

	//LTDC_DitherCmd(ENABLE);
	HAL_LTDC_EnableDither(&LTDC_HandleInitStructure);
	
	/* Display On */
	//LTDC_Cmd(ENABLE);
	LTDC_HandleInitStructure.Instance->GCR|=1;
	
	/* Set opacity */
	//LTDC_LayerAlpha(LTDC_Layer1, 255);
	HAL_LTDC_SetAlpha(&LTDC_HandleInitStructure,255,0);
	//LTDC_LayerAlpha(LTDC_Layer2, 0);
	HAL_LTDC_SetAlpha(&LTDC_HandleInitStructure,0,1);
	
	/* Immediate reload */
	//LTDC_ReloadConfig(LTDC_IMReload);
	__HAL_LTDC_RELOAD_CONFIG(&LTDC_HandleInitStructure);
}
/**********************************************************/
/*         CONFIGURATION of the SCREEN                    */
/**********************************************************/
void SCREEN_SetLayer1(void) {
	SCREEN_Configs.CurrentLayerOffset = 0;
	SCREEN_Configs.CurrentLayer = 0;
}
void SCREEN_SetLayer2(void) {
	SCREEN_Configs.CurrentLayerOffset = LTDC_FRAME_OFFSET;
	SCREEN_Configs.CurrentLayer = 1;
}
void SCREEN_Fill(uint32_t color) {
	uint32_t i;
	uint32_t pixels = SCREEN_PIXEL * 2;
	for (i = 0; i < pixels; i += 2) {
		*(uint16_t *) (SCREEN_FRAME_BUFFER + SCREEN_Configs.CurrentLayerOffset + i) = color;
	}
}
// SCREEN_Config
void SCREEN_Config(){
		//Set cursor X and Y
	SCREEN_x = 0;
	SCREEN_y = 0;
	
	SCREEN_Configs.Width = SCREEN_WIDTH;
	SCREEN_Configs.Height = SCREEN_HEIGHT;
	SCREEN_Configs.Orientation = SCREEN_Portrait;
	SCREEN_Configs.Orient = SCREEN_Orientation_Portrait_1;
	SCREEN_Configs.CurrentLayer = 0;
	SCREEN_Configs.CurrentLayerOffset = 0;
	SCREEN_Configs.Layer1Opacity = 255;
	SCREEN_Configs.Layer2Opacity = 0;
	
	SCREEN_SetLayer1();
	SCREEN_Fill(SCREEN_COLOR_WHITE);
	SCREEN_SetLayer2();
	SCREEN_Fill(SCREEN_COLOR_WHITE);
	SCREEN_SetLayer1();
		
}

/**********************************************************/
/*        INITIALIZATION of the SCREEN                    */
/**********************************************************/

void SCREEN_Init(){
	//	On veut: frequence 180 45 90
	// APB1 45 MHz - APB2 90 MHz
	SCREEN_SystemClock_init();
	//Initialize pins used by the screen
	SCREEN_Pins_init();
	//SPI chip select high
	// c'est PC2 => CSX du LCD-SPI LCD-RGB et LCD-TFT
	SCREEN_CS_SET;
	//Init SPI5 for Screen
	SCREEN_SPI5_Init();
	//SPI5 et Pinpack1
	//Init SDRAM for the screen
	SCREEN_SDRAM_Init();
	//Initialize LCD for LTDC
	SCREEN_LCD_Init();
	//Initialize LTDC
	SCREEN_LTDC_Init();
	//Set data structure for the SCREEN
	SCREEN_Config();
}

