/*
 * cir_flash.cpp
 *
 *  Created on: 16 Jan 2018
 *      Author: christo
 */

#include "../Inc/cir_flash_map.h"

cCirFlashMap::cCirFlashMap(uint32_t sectorSize, uint32_t sectors)
{
	mSectorSize = sectorSize;
	mSectors = sectors - 1;		//index from zero

	mBitShiftCnt = 0;
	while (!(sectorSize & 0x01))
	{
		mBitShiftCnt++;
		sectorSize >>= 1;
	}

	//shift the last bit out
	sectorSize >>= 1;

	//check for any bits
	if (sectorSize | 0x00)
		printf("CIR FLASH sectorSize 0x%06X not on boundry!\n", (unsigned int)mSectorSize);
}

cCirFlashMap::~cCirFlashMap()
{

}

uint32_t cCirFlashMap::getSectorCount()
{
	return mSectors;
}

uint32_t cCirFlashMap::getSectorSize()
{
    return mSectorSize;
}

uint32_t cCirFlashMap::isSectorEnd(uint32_t addr)
{
	uint32_t tmp = mSectorSize-1;
	return ((~addr & tmp) == tmp);
}


HAL_StatusTypeDef cCirFlashMap::getSectorStart(uint32_t *startAddr, uint32_t sectorCount)
{
	if (sectorCount > mSectors)
		return HAL_ERROR;

	if (!sectorCount)
	{
		*startAddr = 0;
		return HAL_OK;
	}

	uint32_t shiftBit = sectorCount << mBitShiftCnt;

	*startAddr = shiftBit;

	return HAL_OK;
}

HAL_StatusTypeDef cCirFlashMap::getSectorEnd(uint32_t *endAddr, uint32_t sectorCount)
{
	if (sectorCount > mSectors)
		return HAL_ERROR;

	sectorCount++;

	uint32_t shiftBit = sectorCount << mBitShiftCnt;

	shiftBit--;
	*endAddr = shiftBit;

	return HAL_OK;
}

HAL_StatusTypeDef cCirFlashMap::getFlashEnd(uint32_t *endAddr)
{
	uint32_t shiftBit = mSectors << mBitShiftCnt;
	*endAddr = shiftBit;

	return HAL_OK;
}
