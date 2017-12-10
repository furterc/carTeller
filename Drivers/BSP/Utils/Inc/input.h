/*
 * input.h
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_INPUT_H_
#define DRIVERS_BSP_UTILS_INPUT_H_

#include <stdbool.h>

#include "stm32l0xx_hal.h"

class cInput {
	uint32_t mPin;
	GPIO_TypeDef *mPort;
public:
	cInput(uint32_t pin, GPIO_TypeDef *port);
	virtual ~cInput();

	bool get();
};

#endif /* DRIVERS_BSP_UTILS_INPUT_H_ */
