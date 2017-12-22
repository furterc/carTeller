/*
 * timer_ic.h
 *
 *  Created on: 17 Dec 2017
 *      Author: christo
 */

#ifndef TIMER_IC_H_
#define TIMER_IC_H_

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_conf.h"

class cTimerIc
{
	TIM_HandleTypeDef htim2;

	uint32_t mStartCCER;
	uint32_t mEndCCER;
	uint32_t mChannel;

	bool mDataAvailable;
	bool mBusy;

public:
	cTimerIc();
	virtual ~cTimerIc();

	void init();
	void ioInit();
	void ioDeInit();
	void startSample(uint32_t channel);
	void handleIrq();
	bool dataReady();
	bool getData(uint32_t *start, uint32_t *end);
};

extern cTimerIc TimerIc;

#endif /* TIMER_IC_H_ */
