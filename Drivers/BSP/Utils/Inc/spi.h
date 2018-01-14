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

#define SPI1_MAX_ADDRESS		  0x7FFFF

#define SPI_OPCODE_WRITE			0x02
#define SPI_OPCODE_READ				0x03
#define SPI_OPCODE_WRITE_DISABLE	0x04
#define SPI_OPCODE_READ_STATUS		0x05
#define SPI_OPCODE_WRITE_ENABLE 	0x06
#define SPI_OPCODE_ERASE_4KB	 	0x20
#define SPI_OPCODE_CHIP_ERASE	 	0x60
#define SPI_OPCODE_ERASE_32KB	 	0x52
#define SPI_OPCODE_ERASE_64KB	 	0xD8
#define SPI_OPCODE_READ_ID			0x9F

class cSPI
{
	SPI_HandleTypeDef hspi;

	void csLow();
	void csHigh();
	HAL_StatusTypeDef writeEnable();
	HAL_StatusTypeDef writeDisable();
	uint32_t SpiFrequency( uint32_t hz );
public:
	cSPI();
	virtual ~cSPI();

	void init(SPI_TypeDef *spiNum, uint32_t frequency);
	HAL_StatusTypeDef readId(uint8_t *data, uint8_t len);
	HAL_StatusTypeDef write(uint32_t address, uint8_t *txData, uint8_t len);
	HAL_StatusTypeDef read(uint32_t address, uint8_t *rxData, uint8_t len);
	HAL_StatusTypeDef erase(uint16_t address, uint8_t blockSizekB);
	HAL_StatusTypeDef chipErase();

};

#endif /* DRIVERS_BSP_UTILS_SPI_H_ */
