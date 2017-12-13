/*
 * ic_timer.h
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#ifndef IC_TIMER_H_
#define IC_TIMER_H_

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_conf.h"
#include "distance_sensor.h"

class cIcTimer
{
//	cDistanceSensor *mSensor1 = 0;
//	cDistanceSensor *mSensor2 = 0;
//	cDistanceSensor *mSensor3 = 0;
//	cDistanceSensor *mSensor4 = 0;
	cDistanceSensor *mSensors[4];
	TIM_HandleTypeDef htim2;
public:
	cIcTimer();
	virtual ~cIcTimer();

	void init();
	void initSensor(uint8_t sensorNumber, cDistanceSensor *sensor);
	void startTimIC();
	void timerIrq();
};

extern cIcTimer IcTimer;

#endif /* IC_TIMER_H_ */
