#ifndef _W25QXXCONFIG_H
#define _W25QXXCONFIG_H

#include <stm32f4xx.h>

#define		_W25QXX_SPI				hspi3
#define		_W25QXX_CS_GPIO			GPIOB
#define		_W25QXX_CS_PIN			GPIO_PIN_0
#define		_W25QXX_USE_FREERTOS	0
#define		_W25QXX_DEBUG			1
#endif
