/*
 * secctor_checker.h
 *
 *  Created on: 23 Feb 2018
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_SECCTOR_CHECKER_H_
#define DRIVERS_BSP_UTILS_SECCTOR_CHECKER_H_

#include <stdint.h>

class cSectorChecker
{
    uint32_t mEntrySize;
    cCirFlashMap *mMap;
    bool mInitialized;

    uint32_t mObjPerSector;
    uint32_t mAvailableBits;
    uint32_t mEntriesPerBit;


public:
    cSectorChecker(uint32_t entrySize, cCirFlashMap *map);
    virtual ~cSectorChecker();

    uint32_t getBytes(uint32_t address, uint8_t *bytes, uint32_t len);
    uint32_t getAddress(uint8_t *bytes, uint32_t len);
};

#endif /* DRIVERS_BSP_UTILS_SECCTOR_CHECKER_H_ */
