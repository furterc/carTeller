/*
 * hw.h
 *
 *  Created on: 22 Dec 2017
 *      Author: christo
 */

#ifndef HW_H_
#define HW_H_

#include "stm32l0xx_hal.h"




class cHw
{
public:
	cHw();
	virtual ~cHw();

	void SystemClock_Config(void);
	uint16_t GetFlashSize(void);
	void Gpio_Init(void);
};

#endif /* HW_H_ */
