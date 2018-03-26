/*
 * spi_device.h
 *
 *  Created on: 17 Jan 2018
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_INC_SPI_DEVICE_H_
#define DRIVERS_BSP_UTILS_INC_SPI_DEVICE_H_

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

#include "spi.h"
#include "output.h"

class cSpiDevice
{
	cSPI *mSPI;
	cOutput *mNssPin;

	void csHigh();
	void csLow();
	HAL_StatusTypeDef writeEnable();
	HAL_StatusTypeDef writeDisable();
public:
	cSpiDevice(cSPI *spi, cOutput *nssPin);
	virtual ~cSpiDevice();

	HAL_StatusTypeDef write(uint32_t address, uint8_t *txData, uint8_t len);
	HAL_StatusTypeDef read(uint32_t address, uint8_t *rxData, uint8_t len);

	HAL_StatusTypeDef erase(uint32_t address, uint8_t blockSizekB);
	HAL_StatusTypeDef chipErase();
	HAL_StatusTypeDef readId(uint8_t *data, uint8_t len);
	HAL_StatusTypeDef readStatus(uint8_t *status);
	HAL_StatusTypeDef isReady();
};

#endif /* DRIVERS_BSP_UTILS_INC_SPI_DEVICE_H_ */
