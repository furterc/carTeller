/*
 * crc.h
 *
 *  Created on: 13 Jan 2018
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_CRC_H_
#define DRIVERS_BSP_UTILS_CRC_H_

#include "stm32l0xx_hal.h"


#define CRC_8_CONSTANT  0xE5

class cCrc
{
public:
	cCrc();
	virtual ~cCrc();

	static uint8_t crc8(uint8_t *data, uint8_t len);
};

#endif /* DRIVERS_BSP_UTILS_CRC_H_ */
