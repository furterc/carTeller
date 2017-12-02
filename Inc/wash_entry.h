/*
 * wash_entry.h
 *
 *  Created on: 28 Nov 2017
 *      Author: christo
 */

#ifndef WASH_ENTRY_H_
#define WASH_ENTRY_H_

#include "rtc.h"
class cWashEntry
{
	RTC_TimeTypeDef mStartTime;
	RTC_TimeTypeDef mEndTime;
	uint8_t mBayNumber;
public:
	cWashEntry(RTC_TimeTypeDef, uint8_t);
	void end(RTC_TimeTypeDef);
	virtual ~cWashEntry();
};

#endif /* WASH_ENTRY_H_ */
