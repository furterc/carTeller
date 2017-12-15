/*
 * distance_sensor.cpp
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#include "distance_sensor.h"
#include "ic_timer.h"
#include "terminal.h"
#include "cmsis_os.h"


cDistanceSensor::cDistanceSensor(uint32_t maxDistance, uint8_t sensorNumber)
{
	mTrigger = 0;

//	printf(GREEN("s: %p distanceTrig\n"), mTrigger);



	mMaxDistance = maxDistance;
	mTickStart = 0;
	mTickEnd = 0;

	mLastSample = 0;
	state = DISTANCE_UNKNOWN;

	mDataAvailable = false;

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

void cDistanceSensor::setTrigger(cOutput *trigger)
{
	mTrigger = trigger;
	mTrigger->reset();
}

void cDistanceSensor::pulse()
{

	CLEAR_BIT(TIM2->CCER, TIM_CCER_CC2P);
//	IcTimer.startTimIC();
//	printf(GREEN("ps: %p pulse\n"), mTrigger);

	printf("%s->%p\n", __FUNCTION__, this);
	mTrigger->set();
	printf(GREEN("s\n"));
	osDelay(1);
	mTrigger->reset();
	printf(GREEN("posrw\n"));
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
		printf("%s->%p\n", __FUNCTION__, this);
		pulse();
		state = DISTANCE_WAIT_ECHO;
	}
		break;
	case DISTANCE_WAIT_ECHO:
	{
		uint8_t cnt = DISTANCE_ECHO_TIMEOUT / 10;
		while (!mDataAvailable && cnt)
		{
			printf(GREEN("w\n"));
			HAL_Delay(10);
			printf(GREEN("w2\n"));
			cnt--;
		}

		if (cnt == 0)
		{
//			if (distanceDebug == 1)
			printf(YELLOW("echo timeout\n"));
			state = DISTANCE_TRIG;
			return;
		}
		printf(GREEN("data available\n"));
		state = DISTANCE_RECEIVE_SAMPLE;
	}
		break;
	case DISTANCE_RECEIVE_SAMPLE:
	{
		static uint8_t sampleCount = 0;
		static uint32_t samples = 0;

		printf("mTickEnd: %0x%04X\n", mTickEnd);
		printf("mTickStart: %0x%04X\n", mTickStart);
		uint32_t distance = mTickEnd - mTickStart;
//		printf("Distance: %0x%04X\n", distance);

//		if ((distance & 0xFFFF0000) == 0xFFFF0000)
//			distance &= ~(0xFFFF0000);
//
//		distance /= 58;
//
//		if (distance > 400)
//			distance = 400;
//
//		samples += distance;
//		sampleCount++;
//		mDataAvailable = false;
//
		state = DISTANCE_WAIT;
//
//		if (sampleCount == 16)
//		{
//			mLastSample = samples >> 4;
////		if (distanceDebug)
////			printf(GREEN("SampleAverage\t: %d\n"), lastSample);
//			samples = 0;
//			sampleCount = 0;
//		}
	}
		break;
	case DISTANCE_WAIT:
	{
		HAL_Delay(DISTANCE_SAMPLE_DUTY);
		state = DISTANCE_TRIG;
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

