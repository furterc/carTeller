#include "cir_flash_map.h"
#include "sector_checker.h"
#include <string.h>

cSectorChecker::cSectorChecker(uint32_t entrySize, cCirFlashMap *map)
{
    mEntrySize = entrySize;
    mMap = map;
    mInitialized = false;
    mObjPerSector = 0;
    mAvailableBits = 0;
    mEntriesPerBit = 0;

    if ((entrySize == 0) || (map == 0))
        return;

    mInitialized = true;

    mObjPerSector = mMap->getSectorSize() / entrySize;
    mAvailableBits = entrySize * 8;
    mAvailableBits--;
    mEntriesPerBit = mObjPerSector / mAvailableBits;

}

cSectorChecker::~cSectorChecker()
{

}

uint32_t cSectorChecker::getBytes(uint32_t address, uint8_t *bytes, uint32_t len)
{
    if (!mInitialized)
        return 0;

    if (len != mEntrySize)
        return 0;

    uint8_t b[len];
    memset(b, 0xFF, len);

    while(address > mMap->getSectorSize())
        address -= mMap->getSectorSize();

    uint32_t bitsToSet = address / ((mEntriesPerBit * mEntrySize) - 1);
    bitsToSet++;

    uint8_t byteCount = 0;
    while(bitsToSet > 8)
    {
        bitsToSet -= 8;
        b[byteCount++] = 0x00;
    }

    for(uint8_t idx = 0; idx < bitsToSet; idx++)
    {
        CLEAR_BIT(b[byteCount], (1 << idx));
    }

    memcpy(bytes, b, len);

    return len;
}

uint32_t cSectorChecker::getNextAddress(uint8_t *bytes, uint32_t len)
{
    uint32_t nextAddress = getAddress(bytes, len);
    nextAddress += (mEntriesPerBit * mEntrySize);
    return nextAddress;
}

uint32_t cSectorChecker::getAddress(uint8_t *bytes, uint32_t len)
{
    if (!mInitialized)
        return 0;

    uint32_t address = 0;

    uint32_t idx = 0;
    while(bytes[idx] == 0x00 && idx < len)
    {
        idx++;
        address += 8 * mEntriesPerBit * mEntrySize;
    }

    //only first bit set;
    if(bytes[idx] == 0xFE)
        return mEntrySize;

    uint32_t idxBit = 1;    //skip first bit
    while(!READ_BIT(bytes[idx], (1 << idxBit)))
    {
        idxBit++;
        address += mEntriesPerBit * mEntrySize;
    }

    return address;
}
