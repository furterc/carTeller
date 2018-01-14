/*
 * log.h
 *
 *  Created on: 12 Jan 2018
 *      Author: christo
 */

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#include <stdio.h>
#include <string.h>

#include "spi.h"
#include "stm32l0xx.h"
#include "terminal.h"
#include "car_wash.h"
#include "crc.h"

/* Sector 0 - Store the device nvm data in the */
#define LOG_NVM_DATA_SECTOR_START		0x000000
#define LOG_NVM_DATA_SECTOR_END			0x00FFFF

/* Sector 1 -> 7 - Store the device nvm data in the */
#define LOG_WASH_DATA_SECTOR_START		0x010000
#define LOG_WASH_DATA_SECTOR_END		0x07FFFF

#define SPI_DEVICE_WHO_AM_I				0x1F8401
#define SPI_DEVICE_WHO_AM_I				0x1F8401



class cLog
{
	cSPI *mSPI = 0;
	bool mInitialized;

	uint32_t mWashDataAddress;
	uint8_t mWashEntrySize;



public:
	cLog(cSPI *spi);
	virtual ~cLog();

	HAL_StatusTypeDef init();
	HAL_StatusTypeDef eraseDevice();

	uint32_t getWashDataAddress();
	HAL_StatusTypeDef getWashEntry(uint32_t addr, sCarwashObject_t *obj);
	HAL_StatusTypeDef addWashEntry(sCarwashObject_t *obj);
};

#endif /* SRC_LOG_H_ */
