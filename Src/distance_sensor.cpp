/*
 * distance_sensor.cpp
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#include "distance_sensor.h"
#include "ic_timer.h"
#include "terminal.h"

cDistanceSensor::cDistanceSensor(cOutput *trigger, uint32_t maxDistance,
		uint8_t sensorNumber)
{
	mTrigger = trigger;

//	printf(GREEN("s: %p distanceTrig\n"), mTrigger);

	mMaxDistance = maxDistance;
	mTickStart = 0;
	mTickEnd = 0;

	mLastSample = 0;
	state = DISTANCE_UNKNOWN;

	mDataAvailable = false;

	mDebug = 0;

	switch (sensorNumber)
	{
	case 1:
		mIcChannel = TIM_CHANNEL_1;
		break;
	case 2:
		mIcChannel = TIM_CHANNEL_2;
		break;
	case 3:
		mIcChannel = TIM_CHANNEL_3;
		break;
	case 4:
		mIcChannel = TIM_CHANNEL_4;
		break;
	default:
		mIcChannel = 0;
		break;
	}
}

cDistanceSensor::~cDistanceSensor()
{

}

void cDistanceSensor::pulse()
{

	CLEAR_BIT(TIM2->CCER, TIM_CCER_CC2P);
	IcTimer.startTimIC();
//	printf(GREEN("ps: %p pulse\n"), mTrigger);

//	printf("%s->%p\n", __FUNCTION__, this);
	mTrigger->set();
//	printf(GREEN("s\n"));
	HAL_Delay(1);
	mTrigger->reset();
//	printf(GREEN("posrw\n"));
}

void cDistanceSensor::setStart(uint32_t start)
{
	mTickStart = start;
	SET_BIT(TIM2->CCER, TIM_CCER_CC2P);
}

void cDistanceSensor::setEnd(uint32_t end)
{
	mTickEnd = end;
	mDataAvailable = true;
}

void cDistanceSensor::setDataAvailable()
{

}

uint32_t cDistanceSensor::getIcChannel()
{
	return mIcChannel;
}

void cDistanceSensor::run()
{

	//printf(GREEN("s: %08X distanceTrig\n"), *mTrigger);
	switch (state)
	{
	case DISTANCE_TRIG:
	{
		pulse();
		state = DISTANCE_WAIT_ECHO;
	}
		break;
	case DISTANCE_WAIT_ECHO:
	{
		static uint32_t tickstart = 0U;

		if (mDataAvailable)
		{
			tickstart = 0;
			state = DISTANCE_RECEIVE_SAMPLE;
			return;
		}

		if (tickstart == 0)
			tickstart = HAL_GetTick();

		if ((HAL_GetTick() - tickstart) > DISTANCE_ECHO_TIMEOUT)
		{
			if (mDebug)
				printf(YELLOW("echo timeout\n"));

			tickstart = 0;
			state = DISTANCE_TRIG;

		}
	}
		break;
	case DISTANCE_RECEIVE_SAMPLE:
	{
		static uint8_t sampleCount = 0;
		static uint32_t samples = 0;

		int distance = mTickEnd - mTickStart;
//		printf("Distance: %0x%04X\n", distance);

		if ((distance & 0xFFFF0000) == 0xFFFF0000)
			distance &= ~(0xFFFF0000);

		distance /= 58;

		if (mDebug == 3)
		{
			printf("atime[0] : 0x%08X\n", (unsigned int) mTickStart);
			printf("atime[1] : 0x%08X\n", (unsigned int) mTickEnd);
			printf(GREEN("distance : 0x%08X\n"), (unsigned int) distance);
		}

		if (distance > 400)
			distance = 400;

		samples += distance;
		sampleCount++;
		mDataAvailable = false;

//		if (mDebug == 2)
			printf("sample: %d\t: %d\n", sampleCount, (unsigned int) distance);

		state = DISTANCE_WAIT;

		if (sampleCount == 16)
		{
			mLastSample = samples >> 4;
			if (mDebug)
			{
				printf(GREEN("SampleAverage\t: %d\n"), mLastSample);
			}
			samples = 0;
			sampleCount = 0;
		}
	}
		break;
	case DISTANCE_WAIT:
	{
		static uint32_t tickstart = 0U;

		if (tickstart == 0)
			tickstart = HAL_GetTick();

		if ((HAL_GetTick() - tickstart) > DISTANCE_SAMPLE_DUTY)
		{
			tickstart = 0;
			state = DISTANCE_TRIG;
		}
	}
		break;
	default:
		state = DISTANCE_WAIT_ECHO;
	}
}

uint16_t cDistanceSensor::getLastSample()
{
	if (!mLastSample)
		return 0;

	uint16_t temp = mLastSample;
	mLastSample = 0;

	return temp;
}

cOutput *cDistanceSensor::getTrigger()
{
	return mTrigger;
}

void cDistanceSensor::setDebug(uint8_t lvl)
{
	mDebug = lvl;
}
