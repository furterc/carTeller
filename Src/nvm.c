/*
 * nvm.c
 *
 *  Created on: 27 Nov 2017
 *      Author: christo
 */

#include "stm32l0xx_hal.h"
#include "nvm.h"
#include <stdio.h>
#include <string.h>

HAL_StatusTypeDef nvm_set(uint32_t Address, uint8_t *data, uint8_t len)
{
    HAL_StatusTypeDef result = HAL_OK;

    result = HAL_FLASHEx_DATAEEPROM_Unlock();
    if (result != HAL_OK)
        return result;

    for (uint8_t k = 0; k < len; k++)
    {
        result = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, Address, data[k]);
        if (result != HAL_OK)
        {
            printf("EERPROM failed to write byte 0x%08X @ 0x%08X\n", (unsigned int) data[k], (unsigned int) Address);
            break;
        }

        Address++;
    }

    HAL_FLASHEx_DATAEEPROM_Lock();
    return result;
}

HAL_StatusTypeDef nvm_getConfig(nvm_config_t *nvm)
{
    memcpy(nvm, (uint8_t*)DATA_EEPROM_BASE, sizeof(nvm_config_t));
    return HAL_OK;
}

HAL_StatusTypeDef nvm_setConfig(nvm_config_t *nvm)
{
    return nvm_set(DATA_EEPROM_BASE, (uint8_t*)nvm, sizeof(nvm_config_t));
}
