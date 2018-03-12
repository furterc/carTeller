/*
 * cir_flash.h
 *
 *  Created on: 16 Jan 2018
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_INC_CIR_FLASH_MAP_H_
#define DRIVERS_BSP_UTILS_INC_CIR_FLASH_MAP_H_

#include <stdio.h>

#include "stm32l0xx_hal.h"

class cCirFlashMap
{
	uint32_t mSectors;
	uint32_t mSectorSize;
	uint8_t mBitShiftCnt;

public:
	cCirFlashMap(uint32_t sectorSize, uint32_t sectors);
	virtual ~cCirFlashMap();

	uint32_t getSectorCount();
	uint32_t getSectorSize();

	uint32_t isSectorBoundry(uint32_t addr);
	uint32_t getAddressSectorStart(uint32_t address);
	uint32_t getSectorStart(uint32_t sectorCount);
	uint32_t getSectorEnd(uint32_t sectorCount);
	uint32_t getFlashEnd();

};

#endif /* DRIVERS_BSP_UTILS_INC_CIR_FLASH_MAP_H_ */
