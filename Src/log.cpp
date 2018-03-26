/*
 * log.cpp
 *
 *  Created on: 12 Jan 2018
 *      Author: christo
 */

#include "log.h"
#include "terminal.h"
#include <stdlib.h>



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

        if(startAddress == 0xFFFFFFFF)
        {
            printf("flash is glo vol?\n");
            mSpiDevice->erase(mCirFlashMap->getSectorStart(sector), 64);
            startAddress = 0;
            break;
        }

        if (startAddress)
        {
            uint32_t offset = mCirFlashMap->getSectorStart(sector);
            startAddress += offset;
        }

        printf("S%d @ 0x%06X: ", (int) mCirFlashMap->isSectorBoundry(address), (int) address);

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

    //no previous entries
    if(!tempHead)
    {
        tempHead = mCirFlashMap->getSectorStart(mStartSector);
        tempHead += mWashEntrySize;
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
    if(address == mCirFlashMap->isSectorBoundry(address))
    {

    }


    uint32_t tempAddr = address;
    sCarwashObject_t tempObj;
    getWashEntry(&tempAddr, &tempObj);

    while (tempObj.ack == 0x00)
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
        if(mCirFlashMap->isSectorBoundry(tempAddr))
        {
            tempAddr += mWashEntrySize;
            getWashEntry(&tempAddr, &tempObj);
        }
    }
    return tempAddr;
}

void cLog::printHeadTail()
{
    printf("LOG HEAD: 0x%08X\n", mHead);
    printf("LOG TAIL: 0x%08X\n", mTail);
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

HAL_StatusTypeDef cLog::ackWashEntry(uint32_t *addr, sCarwashObject_t *obj)
{
    uint32_t sectorStart = mCirFlashMap->getAddressSectorStart(*addr);

    //update the sector checker
    uint8_t bytes[mWashEntrySize];
    mSpiDevice->read(sectorStart, &bytes[0], mWashEntrySize);
    uint32_t nextAddress = mSectorChecker->getNextAddress(bytes, mWashEntrySize);

    //sector full, erase
    if(nextAddress == 0xFFFFFFFF)
    {
        printf("*addr @ 0x%08X is end\n", *addr);
        mSpiDevice->erase(*addr, 64);
//        *addr += mWashEntrySize;
        if (*addr == mCirFlashMap->getFlashEnd())   //end
            *addr = mCirFlashMap->getSectorStart(mStartSector) + mWashEntrySize;
        return HAL_TIMEOUT;
    }

    bool shouldErase = false;
    if(*addr > (nextAddress + sectorStart))
    {
        uint8_t bytes[mWashEntrySize];
        if(mSectorChecker->getBytes(*addr, &bytes[0], mWashEntrySize) == 0xFFFFFFFF)
        {
            shouldErase = true;
        }
        else
            mSpiDevice->write(sectorStart, bytes, mWashEntrySize);
    }

//    //roll over
//    uint32_t boundry = mCirFlashMap->isSectorBoundry(*addr);
//    if(boundry)
//    {
//        boundry--;
//        mSpiDevice->erase(mCirFlashMap->getSectorStart(boundry), 64);
//    }
//        if (*addr == mCirFlashMap->getFlashEnd())
//        {
//
//            *addr = mCirFlashMap->getSectorStart(mStartSector) + mWashEntrySize;
//        }

    // get the object
    if(getWashEntry(addr, obj) != HAL_OK)
        return HAL_ERROR;

    // increment address
    printf("ack%08X\n", (int)*addr);

    if(shouldErase)
    {
        printf("reados *addr @ 0x%08X is end\n", *addr);
        mSpiDevice->erase(*addr, 64);
        *addr += mWashEntrySize;
        if (*addr == mCirFlashMap->getFlashEnd())   //end
        {
            *addr = mCirFlashMap->getSectorStart(mStartSector);
            *addr += mWashEntrySize;
        }
        printf("new *addr @ 0x%08X\n", *addr);
        return HAL_TIMEOUT;
    }

    *addr += mWashEntrySize;
    // clear ack byte
    uint32_t ackPos = *addr - 2;
    uint8_t ack = 0;
    if(mSpiDevice->write(ackPos, &ack, 1) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef cLog::getWashEntry(uint32_t *addr, sCarwashObject_t *obj)
{
    if (!mInitialized)
        return HAL_ERROR;

    uint32_t address = *addr;

    if (address % mWashEntrySize)
    {
        printf("no valid addr..\n");
        return HAL_ERROR;
    }

    //roll over
    if (address == mCirFlashMap->getFlashEnd())
        address = mCirFlashMap->getSectorStart(mStartSector) + mWashEntrySize;

    if (mSpiDevice->read(address, (uint8_t *) obj, mWashEntrySize) != HAL_OK)
        printf(RED("getWasErr"));

    *addr = address;
    /* check the crc */
    uint8_t crc = cCrc::crc8((uint8_t *) obj, mWashEntrySize);
    if (crc)
    {
//		printf(RED("crc @ 0x%08X failed\n"), (unsigned int) address);
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

    if (mSpiDevice->write(mTail, (uint8_t *) obj, mWashEntrySize) != HAL_OK)
    {
        printf("write err @ 0x%08X\n", (unsigned int) mTail);
    }

    mTail += mWashEntrySize;
    if (mTail == mCirFlashMap->getFlashEnd())
        mTail = mCirFlashMap->getSectorStart(mStartSector);

    return HAL_OK;
}

HAL_StatusTypeDef cLog::ackEntries(uint32_t entryCount)
{
    if(mHead == mTail)
        return HAL_ERROR;
    uint32_t addr = mHead;
    static uint32_t endAddr = addr + (entryCount * mWashEntrySize);
    sCarwashObject_t obj;
    while (addr < endAddr)
    {
        if(addr == mTail)
            return HAL_ERROR;

        HAL_StatusTypeDef status = ackWashEntry(&addr, &obj);
        if(status == HAL_TIMEOUT)
        {
            printf("rollover mTail @ 0x%08X\n", (int)mTail);
            endAddr -= mCirFlashMap->getFlashEnd();
        }
        else if(status == HAL_ERROR)
        {
            printf("break mTail @ 0x%08X\n", (int)mTail);
            break;
        }
    }

    mHead = addr;
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
