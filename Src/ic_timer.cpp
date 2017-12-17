/*
 * ic_timer.cpp
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#include "ic_timer.h"
#include "terminal.h"
#include "stm32l0xx.h"
#include "stm32l0xx_hal_conf.h"

cIcTimer::cIcTimer()
{
//	mSensors[0] = mSensor1;
//	mSensors[1] = mSensor2;
//	mSensors[2] = mSensor3;
//	mSensors[3] = mSensor4;

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

//	state = DISTANCE_UNKNOWN;
//	printf(CYAN("Distance initialized\n"));
}

void cIcTimer::initSensor(uint8_t sensorNumber, cDistanceSensor *sensor)
{
//	if ((sensorNumber == 0) || (sensorNumber > 4))
//		return;
//
//	sensorNumber--;
//
////	if (mSensors[sensorNumber] == 0)
//		mSensors[sensorNumber] = sensor;
//
//
//	uint32_t icConfig = 0;
//
//	for (uint8_t idx = 0; idx < 4; idx++)
//	{
//		printf("idx: %d\n", idx);
//		if (mSensors[idx])
//		{
//			printf("p: msens\t\t: 0x%p\n", mSensors[idx]);
////			printf("p: msen\t\t: 0x%p\n", mSensor2);
//			printf("ja: %d\n", idx);
//			icConfig |= mSensors[idx]->getIcChannel();
//		}
//
//	}

	TIM_IC_InitTypeDef sConfigIC;
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_NVIC_SetPriority(TIM2_IRQn, 0x1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void cIcTimer::startTimIC()
{
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);


	printBuff(TIM2->CR1);
	printBuff(TIM2->CR2);
	printBuff(TIM2->CNT);
	printBuff(TIM2->CCMR1);
}

void cIcTimer::timerIrq()
{
	printf("iter2\n");
	if (READ_BIT(TIM2->SR, TIM_SR_CC2IF))
	{
//		if(mSensor2 == 0)
//			return;
		//tweede keer
		if (TIM2->CCER & (1 << 5))
		{
			printf("p: rim\t\t: 0x%p\n", mSensors[1]);
			mSensors[1]->setEnd(TIM2->CCR2);
			mSensors[1]->setDataAvailable();
			return;
		}

		mSensors[1]->setStart(TIM2->CCR2);

	}
}
//extern "C" {
//void TIM2_IRQHandler(void)
//{
//		IcTimer.timerIrq();
//	//	distance_timerIrq();
//}
//}
cIcTimer IcTimer = cIcTimer();

