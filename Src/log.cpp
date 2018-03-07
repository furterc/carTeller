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

cLog::cLog(cSpiDevice *spiDevice, cCirFlashMap *map, uint32_t startSector)
{
    mStartSector = startSector;
    mSpiDevice = spiDevice;
    mInitialized = false;
    mWashEntrySize = sizeof(sCarwashObject_t);
    mCirFlashMap = map;

    mHead = 0;
    mTail = 0;
}

cLog::~cLog()
{
    // TODO Auto-generated destructor stub
}

void cLog::getHeadAndTail()
{
    printf("\n");
    uint32_t tempHead = 0;
    uint8_t refresh = 0;
    for (uint32_t sector = mStartSector; sector < mCirFlashMap->getSectorCount(); sector++)
    {
        uint32_t address = mCirFlashMap->getSectorStart(sector);

        uint8_t bytes[mWashEntrySize];
        mSpiDevice->read(address, bytes, mWashEntrySize);
        uint32_t startAddress = mSectorChecker->getAddress(bytes, mWashEntrySize);

        if (startAddress)
        {
            uint32_t offset = mCirFlashMap->getSectorStart(sector);
            startAddress += offset;
        }

        printf("sector %d @ 0x%06X : ", (int) mCirFlashMap->isSectorBoundry(address), (int) address);

        if (!startAddress)
        {
            printf(RED("No entries found.\n"));
            refresh = 1;
        }
        else
        {
            //begin
            if (refresh)
            {
                refresh = 0;
                tempHead = startAddress;
            }

            if (!tempHead && !refresh)
                tempHead = startAddress;

            printf(GREEN("Entries @ %08X\n"), (unsigned int) startAddress);
        }
    }

    printf(GREEN("TempHead : %08X\n"), (unsigned int) tempHead);

    //no previous entries
    if(!tempHead)
    {
        tempHead = mCirFlashMap->getSectorStart(mStartSector);
        mHead = tempHead;
        mTail = tempHead;
        printf("LOG Head=Tail: 0x%08X\n", (unsigned int) tempHead);
        return;
    }

    mHead = getSectorHead(tempHead);
    printf("LOG Head     : 0x%08X\n", (unsigned int) mHead);
    mTail = getSectorTail(mHead);
    printf("LOG Tail     : 0x%08X\n", (unsigned int) mTail);
}

uint32_t cLog::getSectorHead(uint32_t address)
{
    uint32_t tempAddr = address;
    sCarwashObject_t tempObj;
    getWashEntry(&tempAddr, &tempObj);

    while (cCarWash::checkCrc(&tempObj) != HAL_OK)
    {
        tempAddr += mWashEntrySize;
        getWashEntry(&tempAddr, &tempObj);
    }
    return tempAddr;
}

uint32_t cLog::getSectorTail(uint32_t address)
{
    uint32_t tempAddr = address;
    sCarwashObject_t tempObj;
    getWashEntry(&tempAddr, &tempObj);

    while (cCarWash::checkCrc(&tempObj) == HAL_OK)
    {
        tempAddr += mWashEntrySize;
        getWashEntry(&tempAddr, &tempObj);
    }
    return tempAddr;
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
        mSectorChecker = new cSectorChecker(mWashEntrySize, mCirFlashMap);
        mInitialized = true;
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

HAL_StatusTypeDef cLog::getWashEntry(uint32_t *addr, sCarwashObject_t *obj)
{
    if (!mInitialized)
        return HAL_ERROR;

    if (*addr % mWashEntrySize)
    {
        printf("no valid addr..\n");
        return HAL_ERROR;
    }

    //roll over
    if (*addr == mCirFlashMap->getFlashEnd())
        *addr = getSectorHead(mCirFlashMap->getSectorStart(mStartSector));

    if (mSpiDevice->read(*addr, (uint8_t *) obj, mWashEntrySize) != HAL_OK)
        printf(RED("getWasErr"));

    /* check the crc */
    uint8_t crc = cCrc::crc8((uint8_t *) obj, mWashEntrySize);
    if (crc)
    {
//		printf(RED("crc @ 0x%08X failed\n"), (unsigned int) addr);
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef cLog::addWashEntryAt(uint32_t addr, sCarwashObject_t *obj)
{
    mTail = addr;
    addWashEntry(obj);
    return HAL_OK;
}

HAL_StatusTypeDef cLog::addWashEntry(sCarwashObject_t *obj)
{
    if (mCirFlashMap->isSectorBoundry(mTail) > 0)
    {
        //is hier op 'n boundry wat ek die sector info moet update

        //set eerste bit
        uint8_t tempArr[mWashEntrySize];
        memset(tempArr, 0xFF, mWashEntrySize);
        tempArr[0] &= ~0x01;

        if (mSpiDevice->write(mTail, tempArr, mWashEntrySize)
                != HAL_OK)
        {
            printf("write err @ 0x%08X\n", (unsigned int) mTail);
        }

        //skip
        mTail += mWashEntrySize;
    }

    /* set the crc */
    uint8_t crc = cCrc::crc8((uint8_t *) obj, mWashEntrySize - 1);
    obj->crc = crc;

    if (mSpiDevice->write(mTail, (uint8_t *) obj, mWashEntrySize)
            != HAL_OK)
    {
        printf("write err @ 0x%08X\n", (unsigned int) mTail);
    }

    mTail += mWashEntrySize;
    if (mTail == mCirFlashMap->getFlashEnd())
        mTail = mCirFlashMap->getSectorStart(mStartSector);

    return HAL_OK;
}

void cLog::dumpLog()
{
    uint32_t addr = mHead;
    sCarwashObject_t obj;

    printf(",Date,,,Time,,,Duration\n");
    printf("bay,");
    printf("day,month,year,");
    printf("hour,minute,second,");
    printf("minute,second\n");
    while ((addr < mTail) || (addr < mHead))
    {
        getWashEntry(&addr, &obj);
        printf("addr: 0x%08X -> ", (int)addr);
        printf("%d,", obj.bayNumber);
        printf("%d,%d,20%02d,", obj.date_dayOfMonth, obj.date_monthOfYear, obj.date_year);
        printf("%d,%02d,%02d,", obj.time_hour, obj.time_minute, obj.time_second);
        printf("%d,%02d\n", obj.duration_minute, obj.duration_second);

        addr += mWashEntrySize;
    }
}
