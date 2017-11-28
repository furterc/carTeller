/*
 * nvm.c
 *
 *  Created on: 27 Nov 2017
 *      Author: christo
 */

//#include "stm32l0xx_hal_conf.h"
#include "stm32l0xx_hal.h"
#include "nvm.h"
#include <stdio.h>

bool nvm_erase(uint32_t Address, uint8_t words)
{
	bool result = true;

	HAL_FLASHEx_DATAEEPROM_Unlock();

	for (uint8_t k = 0; k < words; k++)
	{
		if (HAL_FLASHEx_DATAEEPROM_Erase(Address) != HAL_OK)
		{
			printf("EERPROM failed to write byte @ 0x%08X\n", (unsigned int) Address);
			result = false;
			break;
		}

		Address++;
	}

	HAL_FLASHEx_DATAEEPROM_Lock();
	return result;
}

bool nvm_set(uint32_t Address, uint8_t *data, uint8_t len)
{
	bool result = true;

	HAL_FLASHEx_DATAEEPROM_Unlock();

	for (uint8_t k = 0; k < len; k++)
	{
		if (HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, Address, data[0]) != HAL_OK)
		{
			printf("EERPROM failed to write byte 0x%08X @ 0x%08X\n",(unsigned int) data[0], (unsigned int) Address);
			result = false;
			break;
		}

		Address++;
	}

	HAL_FLASHEx_DATAEEPROM_Lock();

	return result;
}


bool nvm_set_CarDistance(uint8_t *distance)
{
	if(nvm_erase(DATA_EEPROM_BASE + NVM_CAR_TRIG_DIST_OFFSET, 1))
		return nvm_set(DATA_EEPROM_BASE + NVM_CAR_TRIG_DIST_OFFSET, distance, 4);

	return false;
}

uint8_t *nvm_get_CarDistance()
{
	return (uint8_t *)(DATA_EEPROM_BASE + NVM_CAR_TRIG_DIST_OFFSET);
}

bool nvm_set_CarTime(uint8_t *distance)
{
	if(nvm_erase(DATA_EEPROM_BASE + NVM_CAR_TRIG_TIME_OFFSET, 1))
		return nvm_set(DATA_EEPROM_BASE + NVM_CAR_TRIG_TIME_OFFSET, distance, 4);

	return false;
}

uint8_t *nvm_get_CarTime()
{
	return (uint8_t *)(DATA_EEPROM_BASE + NVM_CAR_TRIG_TIME_OFFSET);
}
