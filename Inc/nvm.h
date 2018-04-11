/*
 * nvm.h
 *
 *  Created on: 27 Nov 2017
 *      Author: christo
 */

#ifndef INC_NVM_H_
#define INC_NVM_H_

#define NVM_CAR_TRIG_DIST_OFFSET	0
#define NVM_CAR_TRIG_TIME_OFFSET	16

#include <stdbool.h>
#include "terminal.h"

#ifdef __cplusplus
 extern "C" {
#endif

extern sTermEntry_t triggerDistanceEntry;
extern sTermEntry_t triggerTimeEntry;
extern sTermEntry_t minTimeEntry;

typedef struct
{
    uint32_t triggerDistance;
    uint32_t triggerTime;
    uint32_t minimumTime;
} nvm_config_t;

HAL_StatusTypeDef nvm_getConfig(nvm_config_t *nvm);
HAL_StatusTypeDef nvm_setConfig(nvm_config_t *nvm);
void show_nvm();

#ifdef __cplusplus
 }
#endif

#endif /* INC_NVM_H_ */
