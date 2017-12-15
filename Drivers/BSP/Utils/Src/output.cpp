/*
 * output.cpp
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#include "output.h"

cOutput::cOutput(GPIO_TypeDef *port, uint32_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(port, &GPIO_InitStruct);

	mPort = port;
	mPin = pin;
}

cOutput::~cOutput()
{
	HAL_GPIO_DeInit(mPort, mPin);
}

void cOutput::set()
{
	printf("set %p\n", this);
	HAL_GPIO_WritePin(mPort, mPin, GPIO_PIN_SET);
}

void cOutput::reset()
{
	printf("reset %p\n", this);
	HAL_GPIO_WritePin(mPort, mPin, GPIO_PIN_RESET);
}

bool cOutput::get()
{
	if (HAL_GPIO_ReadPin(mPort, mPin) == GPIO_PIN_SET)
		return true;

	return false;
}

