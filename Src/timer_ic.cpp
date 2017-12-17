/*
 * timer_ic.cpp
 *
 *  Created on: 17 Dec 2017
 *      Author: christo
 */

#include "timer_ic.h"

#include <stdbool.h>
#include <stdlib.h>
#include "distance.h"

#include "terminal.h"

cTimerIc::cTimerIc()
{
	// TODO Auto-generated constructor stub

}

cTimerIc::~cTimerIc()
{
	// TODO Auto-generated destructor stub
}

void cTimerIc::init()
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

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

//	HAL_TIM_Base_Start(&htim2);

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

void cTimerIc::ioInit()
{
	printf("was hier\n");

	/* Peripheral clock enable */
	__HAL_RCC_TIM2_CLK_ENABLE();

	/**TIM2 GPIO Configuration
	 PA1     ------> TIM2_CH2
	 */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void cTimerIc::ioDeInit()
{

}

void cTimerIc::startSample(uint32_t channel)
{
	mChannel = channel;
	TIM2->CCER &= ~(1 << 5);

	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, channel);
}

void cTimerIc::handleIrq()
{
	if (READ_BIT(TIM2->SR, TIM_SR_CC2IF))
	{
		if (TIM2->CCER & (1 << 5))
		{
			mEndCCER = TIM2->CCR2;
			mDataAvailable = true;
			WRITE_REG(TIM2->CNT, 0);
			HAL_TIM_Base_Stop(&htim2);
			HAL_TIM_IC_Stop_IT(&htim2, mChannel);
			return;
		}

		mStartCCER = TIM2->CCR2;

		TIM2->CCER |= (1 << 5);
	}
}

bool cTimerIc::dataReady()
{
	if(mDataAvailable)
		return true;

	return false;
}

bool cTimerIc::getData(uint32_t *start, uint32_t *end)
{
	if(mDataAvailable)
	{
		*start = mStartCCER;
		*end = mEndCCER;

		mStartCCER = 0;
		mEndCCER = 0;
		mDataAvailable = false;
		mChannel = 0;
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
