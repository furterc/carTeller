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

uint32_t cCirFlashMap::isSectorBoundry(uint32_t addr)
{
	uint32_t tmp = mSectorSize-1;
	if((~addr & tmp) == tmp)
	    return (addr >> mBitShiftCnt);

	return 0;
}


uint32_t cCirFlashMap::getSectorStart(uint32_t sectorCount)
{
	if (sectorCount > mSectors)
		return 0;

	if (!sectorCount)
		return 0;

	return sectorCount << mBitShiftCnt;
}

uint32_t cCirFlashMap::getSectorEnd(uint32_t sectorCount)
{
	if (sectorCount > mSectors)
		return 0;

	sectorCount++;

	uint32_t shiftBit = sectorCount << mBitShiftCnt;
	return --shiftBit;
}

uint32_t cCirFlashMap::getFlashEnd()
{
	return (mSectors << mBitShiftCnt);
}
