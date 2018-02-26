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
    mEntriesPerBit = mObjPerSector / mAvailableBits;

}

cSectorChecker::~cSectorChecker()
{

}

uint32_t cSectorChecker::getBytes(uint32_t address, uint8_t *bytes, uint32_t len)
{
    printf("1");
    if (!mInitialized)
        return 0;
    printf("2");
    if (len != mEntrySize)
        return 0;
    printf("3");
    uint8_t b[len];
    printf("4");
    memset(b, 0xFF, len);
    printf("5");
    while(address > mMap->getSectorSize())
        address -= mMap->getSectorSize();
    printf("6");
    printf("\n");

    uint32_t bitsToSet = address / (mEntriesPerBit * mEntrySize);
    printf("bitsToSet = %d for %08X\n", bitsToSet, address);

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

    return 0;
//    for(uint8_t)
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

    uint32_t idxBit = 0;
    while(!READ_BIT(bytes[idx], (1 << idxBit)))
    {
        idxBit++;
        address += mEntriesPerBit * mEntrySize;
    }

    return address;
}
