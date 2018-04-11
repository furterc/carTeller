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
#include <stdint.h>
#include <stdlib.h>


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


void show_nvm()
{
    nvm_config_t nvm;
    nvm_getConfig(&nvm);
    printf("Trigger dist : %dcm\n", (int)nvm.triggerDistance);
    printf("Trigger time : %ds\n", (unsigned int)nvm.triggerTime);
    printf("Minimum time : %ds\n", (unsigned int)nvm.minimumTime);
}

void triggerDistance(uint8_t argc, char **argv)
{
    nvm_config_t nvm;
    nvm_getConfig(&nvm);
    if (argc == 1)
    {
        printf("Trigger dist : %dcm\n", (unsigned int)nvm.triggerDistance);
    }
    else if (argc == 2)
    {
        int dist = atoi(argv[1]);
        if(dist < 30 || dist > 450)
        {
            printf(RED("30 < Trigger distance < 450\n"));
            return;
        }

        nvm.triggerDistance = (uint32_t)dist;

        if(nvm_setConfig(&nvm) != HAL_OK)
        {
            printf("failed to update nvm\n");
            return;
        }
        printf(GREEN("Trigger dist : %dcm\n"), (unsigned int)nvm.triggerDistance);
    }
}
sTermEntry_t triggerDistanceEntry =
{ "td", "Set/Get the car trigger distance", triggerDistance };

void triggerTime(uint8_t argc, char **argv)
{
    nvm_config_t nvm;
    nvm_getConfig(&nvm);
    if (argc == 1)
    {
        printf("Trigger time : %ds\n", (unsigned int)nvm.triggerTime);
    }
    else if (argc == 2)
    {
        int time = atoi(argv[1]);
        if(time <= 0)
        {
            printf(RED("Trigger time < 0\n"));
            return;
        }

        nvm.triggerTime = (uint32_t)time;

        if(nvm_setConfig(&nvm) != HAL_OK)
        {
            printf("failed to update nvm\n");
            return;
        }
        printf(GREEN("Trigger time : %ds\n"), (unsigned int)nvm.triggerTime);
    }
}
sTermEntry_t triggerTimeEntry =
{ "tt", "Set/Get the car trigger time", triggerTime };


void minTime(uint8_t argc, char **argv)
{
    nvm_config_t nvm;
    nvm_getConfig(&nvm);
    if (argc == 1)
    {
        printf("Minimum time : %ds\n", (unsigned int)nvm.minimumTime);
    }
    else if (argc == 2)
    {
        int time = atoi(argv[1]);
        if(time <= nvm.triggerTime || time > 60)
        {
            printf(RED("Trigger time(%d)s < Minimum time(s) < 60s\n"), (int)nvm.triggerTime);
            return;
        }

        nvm.minimumTime = (uint32_t)time;

        if(nvm_setConfig(&nvm) != HAL_OK)
        {
            printf("failed to update nvm\n");
            return;
        }
        printf(GREEN("Minimum time : %ds\n"), (unsigned int)nvm.minimumTime);
    }
}
sTermEntry_t minTimeEntry =
{ "tm", "Set/Get the minimum carwash time", minTime };
