/*
 * ic_timer.cpp
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#include "ic_timer.h"
#include "terminal.h"
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"

cIcTimer::cIcTimer()
{
	mSensors[0] = mSensor1;
	mSensors[1] = mSensor2;
	mSensors[2] = mSensor3;
	mSensors[3] = mSensor4;

}

cIcTimer::~cIcTimer()
{
}

void cIcTimer::init()
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0x20;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFF;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_TIM_Base_Start(&htim2);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_NVIC_SetPriority(TIM2_IRQn, 0x1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

//	state = DISTANCE_UNKNOWN;
//	printf(CYAN("Distance initialized\n"));
}

void cIcTimer::initSensor(uint8_t sensorNumber, cDistanceSensor *sensor)
{
	printf("1\n");
	if ((sensorNumber == 0) || (sensorNumber > 4))
		return;

	printf("2\n");
	sensorNumber--;

	printf("3\n");
	if (mSensors[sensorNumber] == 0)
		mSensors[sensorNumber] = sensor;

	printf("4\n");
	uint32_t icConfig = 0;

	for (uint8_t idx = 0; idx < 4; idx++)
	{
		if (mSensors[idx])
			icConfig |= mSensors[idx]->getIcChannel();
	}

	printf("5\n");
	TIM_IC_InitTypeDef sConfigIC;
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	printf("6\n");
	HAL_TIM_Base_Start(&htim2);
	printf("7\n");
	printf("kak\n");
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
	printf("kak\n");
	printf("8\n");
}

void cIcTimer::startTimIC()
{

}

void cIcTimer::timerIrq()
{
	if (READ_BIT(TIM2->SR, TIM_SR_CC2IF))
	{
		if(mSensor2 == 0)
			return;
		//tweede keer
		if (TIM2->CCER & (1 << 5))
		{
			mSensor2->setEnd(TIM2->CCR2);
			mSensor2->setDataAvailable();
			return;
		}

		mSensor2->setStart(TIM2->CCR2);

	}

}
//extern "C" {
//void TIM2_IRQHandler(void)
//{
//	//	IcTimer.timerIrq();
//	//	distance_timerIrq();
//}
//}
cIcTimer IcTimer = cIcTimer();

