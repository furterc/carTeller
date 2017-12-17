/*
 * ultra_s_sensor.cpp
 *
 *  Created on: 17 Dec 2017
 *      Author: christo
 */

#include "ultra_s_sensor.h"
#include "terminal.h"

void cUltraSSensor::pulse()
{
	if (mTrigger == 0)
		return;

	mTimer->startSample(TIM_CHANNEL_2);

	mTrigger->set();
	HAL_Delay(1);
	mTrigger->reset();
}

cUltraSSensor::cUltraSSensor()
{
	mTimer = 0;
	mTrigger = 0;
	mState = DISTANCE_UNKNOWN;

	mDebug = 0;
	mLastSample = 0;
}

cUltraSSensor::~cUltraSSensor()
{
	// TODO Auto-generated destructor stub
}

void cUltraSSensor::init(cTimerIc *timer, cOutput *trigger)
{
	mTimer = timer;
	mTrigger = trigger;
	mTimer->init();
}

void cUltraSSensor::setDebug(uint8_t debug)
{
	mDebug = debug;
}

void cUltraSSensor::run()
{
	switch (mState)
	{
	case DISTANCE_TRIG:
	{
		pulse();
		mState = DISTANCE_WAIT_ECHO;
	}
		break;
	case DISTANCE_WAIT_ECHO:
	{
		static uint32_t tickstart = 0U;

		if(mTimer->dataReady())
		{
			tickstart = 0;
			mState = DISTANCE_RECEIVE_SAMPLE;
			return;
		}

		if (tickstart == 0)
			tickstart = HAL_GetTick();

		if((HAL_GetTick() - tickstart) > DISTANCE_ECHO_TIMEOUT)
		{
			if(mDebug)
				printf(YELLOW("echo timeout\n"));

			tickstart = 0;
			mState = DISTANCE_TRIG;

		}
	}
		break;
	case DISTANCE_RECEIVE_SAMPLE:
	{
		static uint8_t sampleCount = 0;
		static uint32_t samples = 0;

		uint32_t start = 0;
		uint32_t end = 0;
		mTimer->getData(&start, &end);

		int distance = end - start;

		if ((distance & 0xFFFF0000) == 0xFFFF0000)
		{
			distance &= ~(0xFFFF0000);
		}

		if (mDebug == 3)
		{
			printf("atime[0] : 0x%08X\n", (unsigned int) start);
			printf("atime[1] : 0x%08X\n", (unsigned int) end);
			printf(GREEN("distance : 0x%08X\n"), (unsigned int) distance);
		}

		distance /= 58;

		if (distance > 400)
			distance = 400;


		samples += distance;
		sampleCount++;

		if (mDebug == 2)
			printf("sample: %d\t: %d\n", sampleCount, (unsigned int) distance);

		mState = DISTANCE_WAIT;

		if (sampleCount == 16)
		{
			mLastSample = samples >> 4;
			if (mDebug)
				printf(GREEN("SampleAverage\t: %d\n"), (unsigned int)mLastSample);
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
			mState = DISTANCE_TRIG;
		}
	}
		break;
	default:
		mState = DISTANCE_WAIT_ECHO;
	}

}
