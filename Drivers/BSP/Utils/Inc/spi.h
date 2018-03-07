/*
 * spi.h
 *
 *  Created on: 07 Dec 2017
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_SPI_H_
#define DRIVERS_BSP_UTILS_SPI_H_

#include "stdint.h"
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_conf.h"


/**SPI1 GPIO Configuration
 * PA4  --> SPI1_NSS
 * PA6  --> SPI1_MISO
 * PA7  --> SPI1_MOSI
 * PB3  --> SPI1_SCK
 */
#define SPI1_GPIO_NSS_PIN         GPIO_PIN_4
#define SPI1_GPIO_NSS_PORT        GPIOA
#define SPI1_GPIO_MISO_PIN        GPIO_PIN_6
#define SPI1_GPIO_MISO_PORT       GPIOA
#define SPI1_GPIO_MOSI_PIN        GPIO_PIN_7
#define SPI1_GPIO_MOSI_PORT       GPIOA
#define SPI1_GPIO_SCK_PIN         GPIO_PIN_3
#define SPI1_GPIO_SCK_PORT        GPIOB
#define SPI1_GPIO_AF			  GPIO_AF0_SPI1


class cSPI
{
	SPI_HandleTypeDef hspi;
	bool mInitialized;

	uint32_t SpiFrequency( uint32_t hz );

public:
	cSPI();
	virtual ~cSPI();

	HAL_StatusTypeDef init(SPI_TypeDef *spiNum, uint32_t frequency);

	HAL_StatusTypeDef write(uint8_t *txData, uint8_t len);
	HAL_StatusTypeDef read(uint8_t *rxData, uint8_t len);

	HAL_StatusTypeDef writeOpCode(uint8_t opCode);
	HAL_StatusTypeDef writeOpCodeAt(uint32_t address, uint8_t opCode);
	HAL_StatusTypeDef readOpCode(uint8_t opCode, uint8_t *data, uint8_t len);
	HAL_StatusTypeDef readOpCodeAt(uint32_t address, uint8_t opCode, uint8_t *data, uint8_t len);
};

#endif /* DRIVERS_BSP_UTILS_SPI_H_ */
