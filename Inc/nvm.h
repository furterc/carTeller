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

bool nvm_erase(uint32_t Address, uint8_t words);
bool nvm_set(uint32_t Address, uint8_t *data, uint8_t words);

bool nvm_set_CarDistance(uint8_t *distance);
uint8_t *nvm_get_CarDistance();

bool nvm_set_CarTime(uint8_t *distance);
uint8_t *nvm_get_CarTime();

#endif /* INC_NVM_H_ */
