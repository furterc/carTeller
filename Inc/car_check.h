/*
 * car_check.h
 *
 *  Created on: 20 Dec 2017
 *      Author: christo
 */

#include <stdint-gcc.h>

#include "rtc.h"
#include "car_wash.h"

#ifndef CAR_CHECK_H_
#define CAR_CHECK_H_

class cCarCheck
{
	typedef enum
	{
		CARCHECK_STATE_UNKNOWN,
		CARCHECK_STATE_IDLE,
		CARCHECK_STATE_DETECT,
		CARCHECK_STATE_START,
		CARCHECK_STATE_INPROGRESS,
		CARCHECK_STATE_END
	} carCheckState_t;

	uint32_t mTriggerDistance;
	uint32_t mTriggerTime;
	uint8_t mCarCount;
	uint8_t mBayNumber;

	carCheckState_t carState = CARCHECK_STATE_UNKNOWN;
	cCarWash *mCarWash = 0;
	RTC_TimeTypeDef mStartTime;


public:
	cCarCheck(uint32_t triggerDistance, uint32_t triggerTime, uint8_t bayNumber);
	virtual ~cCarCheck();

	uint8_t run(uint32_t distance);
	cCarWash *getCarWash();
};

#endif /* CAR_CHECK_H_ */
