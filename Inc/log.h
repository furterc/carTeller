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

#include "spi_device.h"
#include "cir_flash_map.h"
#include "sector_checker.h"
#include "stm32l0xx.h"
#include "terminal.h"
#include "car_wash.h"
#include "crc.h"

/* Sector 0 - Store the device nvm data */
#define LOG_NVM_DATA_SECTOR_START		0x000000
#define LOG_NVM_DATA_SECTOR_END			0x00FFFF

/* Sector 1 -> 7 - Store the log data */
#define LOG_WASH_DATA_SECTOR_START		0x010000
#define LOG_WASH_DATA_SECTOR_END		0x07FFFF

#define SPI_DEVICE_WHO_AM_I				0x1F8401
#define SPI_DEVICE_WHO_AM_I				0x1F8401

class cLog
{
	cSpiDevice *mSpiDevice = 0;
	cCirFlashMap *mCirFlashMap = 0;
	cSectorChecker *mSectorChecker = 0;
	bool mInitialized;

	uint32_t mWashDataAddress;
	uint8_t mWashEntrySize;

	uint32_t getSectorStatus(uint32_t sector);
	uint32_t setSectorStatus(uint32_t sector, uint32_t address);

	uint32_t mHead;
	uint32_t mTail;

	void getHeadAndTail();
public:
	cLog(cSpiDevice *spiDevice, uint32_t sectorSize, uint32_t startSector, uint32_t sectors);
	virtual ~cLog();

	HAL_StatusTypeDef init();
	HAL_StatusTypeDef eraseDevice();

	uint32_t getWashDataAddress();
	HAL_StatusTypeDef getWashEntry(uint32_t addr, sCarwashObject_t *obj);
	HAL_StatusTypeDef addWashEntry(sCarwashObject_t *obj);
	void dumpLog();
};

#endif /* SRC_LOG_H_ */
