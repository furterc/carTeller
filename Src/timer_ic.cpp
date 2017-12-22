/*
 * timer_ic.cpp
 *
 *  Created on: 17 Dec 2017
 *      Author: christo
 */

#include "timer_ic.h"

#include <stdbool.h>
#include <stdlib.h>

#include "terminal.h"

cTimerIc::cTimerIc()
{
	mStartCCER = 0;
	mEndCCER = 0;
	mChannel = 0;
	mDataAvailable = false;
	mBusy = false;
}

cTimerIc::~cTimerIc()
{
}

void cTimerIc::init()
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	ioInit();

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
	printf("IC init\n");

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	HAL_NVIC_SetPriority(TIM2_IRQn, 0x1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void cTimerIc::ioInit()
{
	/* Peripheral clock enable */
	__HAL_RCC_TIM2_CLK_ENABLE();

	/**TIM2 GPIO Configuration
	 PA1     ------> TIM2_CH2
	 */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_1;// | GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/**TIM2 GPIO Configuration
	 PA10     ------> TIM2_CH3
	 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void cTimerIc::ioDeInit()
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
}

void cTimerIc::startSample(uint32_t channel)
{
	if (mBusy)
		return;

	mBusy = true;

	TIM_IC_InitTypeDef sConfigIC;
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, channel) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	mChannel = channel;

	CLEAR_BIT(TIM2->CCER, TIM_CCER_CC2P);
	CLEAR_BIT(TIM2->CCER, TIM_CCER_CC3P);

	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, channel);
}

void cTimerIc::handleIrq()
{
	if (mChannel == TIM_CHANNEL_2)
	{
		if (READ_BIT(TIM2->CCER, TIM_CCER_CC2P))
		{
			mEndCCER = TIM2->CCR2;
			mDataAvailable = true;
			HAL_TIM_Base_Stop(&htim2);
			HAL_TIM_IC_Stop_IT(&htim2, mChannel);
			return;
		}
		mStartCCER = TIM2->CCR2;
		SET_BIT(TIM2->CCER, TIM_CCER_CC2P);
		return;
	}

	if (mChannel == TIM_CHANNEL_3)
	{
		if (READ_BIT(TIM2->CCER, TIM_CCER_CC3P))
		{
			mEndCCER = TIM2->CCR3;
			mDataAvailable = true;
			HAL_TIM_Base_Stop(&htim2);
			HAL_TIM_IC_Stop_IT(&htim2, mChannel);
			return;
		}
		SET_BIT(TIM2->CCER, TIM_CCER_CC3P);
		mStartCCER = TIM2->CCR3;
	}
}

bool cTimerIc::dataReady()
{
	if (mDataAvailable)
		return true;

	return false;
}

bool cTimerIc::getData(uint32_t *start, uint32_t *end)
{
	if (mDataAvailable)
	{
		*start = mStartCCER;
		*end = mEndCCER;

		mStartCCER = 0;
		mEndCCER = 0;
		mDataAvailable = false;
		mChannel = 0;
		mBusy = false;
		return true;
	}
	return false;
}

extern "C"
{
void TIM2_IRQHandler(void)
{
	TimerIc.handleIrq();
}
}

cTimerIc TimerIc = cTimerIc();
