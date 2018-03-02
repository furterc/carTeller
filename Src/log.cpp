/*
 * log.cpp
 *
 *  Created on: 12 Jan 2018
 *      Author: christo
 */

#include "log.h"
#include "terminal.h"
#include <stdlib.h>

//uint32_t getSectorStatus(uint32_t sector)
//{
//
//}
//
//uint32_t setSectorStatus(uint32_t sector, uint32_t address)
//{
//
//}

cLog::cLog(cSpiDevice *spiDevice, uint32_t sectorSize, uint32_t startSector, uint32_t sectors)
{
	mSpiDevice = spiDevice;
	mInitialized = false;
	mWashDataAddress = 0;
	mWashEntrySize = sizeof(sCarwashObject_t);
	mCirFlashMap = new cCirFlashMap(sectorSize, sectors);


	mHead = 0;
	mTail = 0;
}

cLog::~cLog()
{
	// TODO Auto-generated destructor stub
}

void cLog::getHeadAndTail()
{
//TODO moet elke start kyk vir data bits.

	uint32_t tempAddress = LOG_WASH_DATA_SECTOR_START;

	sCarwashObject_t tempObj;
	getWashEntry(tempAddress, &tempObj);
	printf("1");

//	// soek begin
//	while (cCarWash::checkCrc(&tempObj) == HAL_ERROR)
//	{
//		tempAddress += mWashEntrySize;
//		getWashEntry(tempAddress, &tempObj);
//	}
	mHead = tempAddress;

	while (cCarWash::checkCrc(&tempObj) == HAL_OK)
	{
		tempAddress += mWashEntrySize;
		getWashEntry(tempAddress, &tempObj);
	}
	mTail= tempAddress;
	printf("LOG Head     : 0x%08X\n", (unsigned int)mHead);
	printf("LOG Tail     : 0x%08X\n", (unsigned int)mTail);
//	return tempAddress;
}

uint32_t cLog::getWashDataAddress()
{
	uint32_t tempAddress = LOG_WASH_DATA_SECTOR_START;

	sCarwashObject_t tempObj;
	getWashEntry(tempAddress, &tempObj);

	while (tempObj.entry_set != 0xFF)
	{
		tempAddress += mWashEntrySize;
		getWashEntry(tempAddress, &tempObj);

		if (tempAddress > LOG_WASH_DATA_SECTOR_END)
			return 0;
	}
	return tempAddress;
}

HAL_StatusTypeDef cLog::init()
{
	uint8_t data[3];
	mSpiDevice->readId(data, 3);

	uint32_t deviceId = 0;
	deviceId |= ((data[0] & 0xFF) << 16);
	deviceId |= ((data[1] & 0xFF) << 8);
	deviceId |= (data[2] & 0xFF);

	if (deviceId == SPI_DEVICE_WHO_AM_I)
	{
//	    mSectorChecker = new cSectorChecker(mWashEntrySize, mCirFlashMap);
//	    uint8_t bytes[mWashEntrySize];
//	    mSectorChecker->getBytes(0x4F00F, bytes, mWashEntrySize);
//	    printf("bits:");
//	    for(uint8_t i = 0; i < mWashEntrySize; i++)
//	    {
//	        printf(" %02X,", bytes[i]);
//	    }
//	    printf("\n");
//
//	    printf("ek sal @ %08X moet begin lees\n", mSectorChecker->getAddress(bytes, mWashEntrySize));

		mInitialized = true;
		mWashDataAddress = getWashDataAddress();
		getHeadAndTail();
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef cLog::eraseDevice()
{
	if (!mInitialized)
		return HAL_ERROR;

	printf(RED("Erasing SPI device\n"));

	mSpiDevice->chipErase();

	return HAL_OK;
}

HAL_StatusTypeDef cLog::getWashEntry(uint32_t addr, sCarwashObject_t *obj)
{
	if (!mInitialized)
		return HAL_ERROR;

	if (addr % mWashEntrySize)
	{
		printf("no valid addr..\n");
		return HAL_ERROR;
	}

	if (mSpiDevice->read(addr, (uint8_t *) obj, mWashEntrySize) != HAL_OK)
		printf(RED("getWasErr"));

	/* check the crc */
	uint8_t crc = cCrc::crc8((uint8_t *) obj, mWashEntrySize);
	if (crc)
	{
		printf(RED("crc @ 0x%08X failed\n"), (unsigned int) addr);
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef cLog::addWashEntry(sCarwashObject_t *obj)
{
	/* set the crc */
	uint8_t crc = cCrc::crc8((uint8_t *) obj, mWashEntrySize - 1);
	obj->crc = crc;

	if (mSpiDevice->write(mWashDataAddress, (uint8_t *) obj, mWashEntrySize)
			!= HAL_OK)
	{
		printf("write err @ 0x%08X\n", (unsigned int) mWashDataAddress);
	}

	mWashDataAddress += mWashEntrySize;

	return HAL_OK;
}

void cLog::dumpLog()
{
	uint32_t addr = LOG_WASH_DATA_SECTOR_START;
	sCarwashObject_t obj;


	printf(",Date,,,Time,,,Duration\n");
	printf("bay,");
	printf("day,month,year,");
	printf("hour,minute,second,");
	printf("minute,second\n");
	while (addr < mWashDataAddress)
	{
		getWashEntry(addr, &obj);
		printf("%d,", obj.bayNumber);
		printf("%d,%d,20%02d,", obj.date_dayOfMonth, obj.date_monthOfYear, obj.date_year);
		printf("%d,%02d,%02d,", obj.time_hour, obj.time_minute, obj.time_second);
		printf("%d,%02d\n", obj.duration_minute, obj.duration_second);

		addr += mWashEntrySize;
	}
}
