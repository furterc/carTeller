/*
 * output.h
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_OUTPUT_H_
#define DRIVERS_BSP_UTILS_OUTPUT_H_

#include <stdbool.h>

#include "stm32l0xx_hal.h"
class cOutput {
	uint32_t mPin;
	GPIO_TypeDef *mPort;
public:
	cOutput(uint32_t pin, GPIO_TypeDef *port);
	virtual ~cOutput();

	void set();
	void reset();
	bool get();
};

#endif /* DRIVERS_BSP_UTILS_OUTPUT_H_ */
