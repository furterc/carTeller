/*
 * car_check.cpp
 *
 *  Created on: 20 Dec 2017
 *      Author: christo
 */

#include "terminal.h"
#include "car_check.h"
#include "nvm.h"

cCarCheck::cCarCheck(uint32_t triggerDistance, uint32_t triggerTime, uint8_t bayNumber)
{
	mTriggerDistance = triggerDistance;
	mTriggerTime = triggerTime;
	mBayNumber = bayNumber;
	mCarCount = 0;
}

cCarCheck::~cCarCheck()
{

}

uint8_t cCarCheck::run(uint32_t distance)
{
	switch (carState)
	{
	case CARCHECK_STATE_IDLE:
		mCarCount = 0;

		/* car detected */
		if (distance < mTriggerDistance)
		{
			mStartTime =cRTC::getInstance()->getTime();
			carState = CARCHECK_STATE_DETECT;
		}


		break;
		/* car detected */
	case CARCHECK_STATE_DETECT:
	{
		/* that was no car */
		if (distance > mTriggerDistance)
		{
			carState = CARCHECK_STATE_IDLE;

			if(mCarWash)
			{
				delete(mCarWash);
				mCarWash = 0;
			}
			return 0;
		}

		mCarCount++;

		/* this is a car */
		if (mCarCount > mTriggerTime)
		{
			RTC_DateTypeDef startDate = cRTC::getInstance()->getDate();

			//if there is not a carwash obj, make one
			if(!mCarWash)
				mCarWash = new cCarWash(mBayNumber, startDate.Date, startDate.Month, startDate.Year);

			mCarWash->start(mStartTime.Hours, mStartTime.Minutes, mStartTime.Seconds);

			printf(GREEN("Car in bay: %d @ %02d:%02d:%02d\n"), mBayNumber, mStartTime.Hours,
					mStartTime.Minutes, mStartTime.Seconds);
			mCarCount = 0;
			carState = CARCHECK_STATE_INPROGRESS;
		}
	}
		break;
		/* car in bay */
	case CARCHECK_STATE_INPROGRESS:
		if (distance > mTriggerDistance)
		{
			mCarCount++;
			if(mCarCount > mTriggerTime)
			{
				mCarCount = 0;
				carState = CARCHECK_STATE_END;
				return 0;
			}
			return 0;
		}
		mCarCount = 0;
		break;
		/* car got away */
	case CARCHECK_STATE_END:
	{
		RTC_TimeTypeDef endTime = cRTC::getInstance()->getTime();
		mCarWash->end(endTime.Hours, endTime.Minutes, endTime.Seconds);

		carState = CARCHECK_STATE_IDLE;
		return 1;
	}
		break;
	default:
		carState = CARCHECK_STATE_IDLE;
		break;
	}

	return 0;
}

cCarWash *cCarCheck::getCarWash()
{
	cCarWash *temp = mCarWash;
	mCarWash = 0;
	return temp;
}
