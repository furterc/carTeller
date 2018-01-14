/*
 * log.cpp
 *
 *  Created on: 12 Jan 2018
 *      Author: christo
 */

#include "log.h"

cLog::cLog(cSPI *spi)
{
	mSPI = spi;
	mInitialized = false;
	mWashDataAddress = 0;
	mWashEntrySize = sizeof(sCarwashObject_t);

}

cLog::~cLog()
{
	// TODO Auto-generated destructor stub
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
	mSPI->readId(data, 3);

	uint32_t deviceId = 0;
	deviceId |= ((data[0] & 0xFF) << 16);
	deviceId |= ((data[1] & 0xFF) << 8);
	deviceId |= (data[2] & 0xFF);

	if (deviceId == SPI_DEVICE_WHO_AM_I)
	{

		mInitialized = true;
		mWashDataAddress = getWashDataAddress();
		return HAL_OK;
	}
	return HAL_ERROR;
}

HAL_StatusTypeDef cLog::eraseDevice()
{
	if (!mInitialized)
		return HAL_ERROR;

	printf(RED("Erasing SPI device\n"));

	mSPI->chipErase();

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

	if (mSPI->read(addr, (uint8_t *) obj, mWashEntrySize) != HAL_OK)
		printf(RED("getWasErr"));

//	uint8_t crc = cCrc::crc8((uint8_t *)obj, mWashEntrySize);
//	printf("crc: 0x%02X\n", crc);

	return HAL_OK;
}

HAL_StatusTypeDef cLog::addWashEntry(sCarwashObject_t *obj)
{
	//set the crc
//	uint8_t crc = cCrc::crc8((uint8_t *)obj, mWashEntrySize-1);
//	obj->crc = crc;

	if (mSPI->write(mWashDataAddress, (uint8_t *) obj, mWashEntrySize)
			!= HAL_OK)
	{
		printf("write err @ 0x%08X\n", (unsigned int) mWashDataAddress);
	}

	mWashDataAddress += mWashEntrySize;

	return HAL_OK;
}
