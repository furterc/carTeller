/*
 * distance.c
 *
 *  Created on: 25 Nov 2017
 *      Author: christo
 */
#include <stdbool.h>
#include <stdlib.h>
#include "distance.h"
#include "stm32l0xx_hal.h"
#include "terminal.h"
#include "cmsis_os.h"

#define DISTANCE_ECHO_TIMEOUT		2000		//startup time
#define DISTANCE_SAMPLE_DUTY		1000/16  	//16 samples a second
#define DISTANCE_MAX				400*58		//450cm * multiplier
#define DISTANCE_TIMEOUT			100			//ms

TIM_HandleTypeDef htim2;
distanceStates_t state = DISTANCE_UNKNOWN;

uint16_t atime[2];
bool dataAvailable = false;
int lastSample = 0;

uint8_t distanceDebug = 0;

void distance_Init()
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

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

	state = DISTANCE_UNKNOWN;
	printf(CYAN("Distance initialized\n"));
}

void distance_IoInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Configure GPIO pin : P1_Pin */
	GPIO_InitStruct.Pin = P1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(P1_GPIO_Port, &GPIO_InitStruct);

	/* Peripheral clock enable */
	__HAL_RCC_TIM2_CLK_ENABLE();

	/**TIM2 GPIO Configuration
	 PA1     ------> TIM2_CH2
	 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void distance_IoDeInit()
{
	HAL_GPIO_DeInit(P1_GPIO_Port, P1_Pin);

	/* Peripheral clock disable */
	__HAL_RCC_TIM2_CLK_DISABLE();

	/**TIM2 GPIO Configuration
	 PA0     ------> TIM2_CH1
	 */
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
}

void distance_pulse()
{
	TIM2->CNT = 0;
	// capture on rising edge
	TIM2->CCER &= ~(1 << 5);

	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);

	// pulse the trig pin
	HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_SET);
	osDelay(1);
	HAL_GPIO_WritePin(P1_GPIO_Port, P1_Pin, GPIO_PIN_RESET);
}

void distance_timerIrq()
{
	static uint8_t overflowCnt = 0;
	if (TIM2->SR & TIM_FLAG_UPDATE)
	{
		//clear the flag
		TIM2->SR &= ~TIM_FLAG_UPDATE;

		//timer overflow
		if (overflowCnt++ > 5)
		{
			if (distanceDebug)
				printf("overflow!\n");

			atime[1] = atime[0] + DISTANCE_MAX;
			dataAvailable = true;
			overflowCnt = 0;
		}
		return;
	}

	overflowCnt = 0;
	if (TIM2->CCER & (1 << 5))
	{
		atime[1] = TIM2->CCR2;
		dataAvailable = true;
		return;
	}

	atime[0] = TIM2->CCR2;

	TIM2->CCER |= (1 << 5);
}

void distance_run()
{
	switch (state)
	{
		case DISTANCE_TRIG:
		{
			distance_pulse();
			state = DISTANCE_WAIT_ECHO;
		}
		break;
		case DISTANCE_WAIT_ECHO:
		{
			uint8_t cnt = DISTANCE_ECHO_TIMEOUT/10;
			while(!dataAvailable && cnt)
			{
				osDelay(10);
				cnt--;
			}

			if (cnt == 0)
			{
				if (distanceDebug == 1)
					printf(YELLOW("echo timeout\n"));
				state = DISTANCE_TRIG;
				return;
			}

			state = DISTANCE_RECEIVE_SAMPLE;
		}
		break;
		case DISTANCE_RECEIVE_SAMPLE:
		{
			static uint8_t sampleCount = 0;
			static int samples = 0;

			int distance = atime[1] - atime[0];
			distance /= 58;

			if (distance > 400)
				distance = 400;

			samples += distance;
			sampleCount++;
			dataAvailable = false;

			if (distanceDebug == 2)
				printf("sample: %d\t: %d\n", sampleCount, distance);

			state = DISTANCE_WAIT;

			if (sampleCount == 16)
			{
				lastSample = samples >> 4;
				if (distanceDebug)
					printf(GREEN("SampleAverage\t: %d\n"), lastSample);
				samples = 0;
				sampleCount = 0;
			}
		}
		break;
		case DISTANCE_WAIT:
		{
			osDelay(DISTANCE_SAMPLE_DUTY);
			state = DISTANCE_TRIG;
		}
		break;
		default:
			state = DISTANCE_WAIT_ECHO;
	}
}

uint8_t distance_getLastSample(int *sample)
{
	if (!lastSample)
		return 0;

	*sample = lastSample;
	lastSample = 0;
	return 1;
}


void distance_debug(uint8_t argc, char **argv)
{
	if (argc != 2)
	{
		printf("n - debug lvl enabled \n\r0 - debug disabled\n");
		return;
	}

	uint8_t lvl = atoi(argv[1]);

	if (lvl > 2)
		lvl = 2;

	if (lvl < 0)
		lvl = 0;

	distanceDebug = lvl;
}

sTermEntry_t ddebugEntry =
{ "dd", "distanceDebug", distance_debug};
