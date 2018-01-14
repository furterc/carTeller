/*
 * crc.cpp
 *
 *  Created on: 13 Jan 2018
 *      Author: christo
 */

#include "crc.h"

cCrc::cCrc()
{
	// TODO Auto-generated constructor stub

}

cCrc::~cCrc()
{
	// TODO Auto-generated destructor stub
}

uint8_t cCrc::crc8(uint8_t *data, uint8_t len)
{
	uint8_t crc = 0x00;
	uint8_t idx = 0x00;
	while(idx < len)
	{
		crc ^= data[idx];
		for (uint8_t i = 0; i < 8; i++)
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ CRC_8_CONSTANT;
			else
				crc <<= 1;
		}
		idx++;
	}
	return crc;
}
