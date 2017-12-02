/*
 * wash_entry.cpp
 *
 *  Created on: 28 Nov 2017
 *      Author: christo
 */

#include "wash_entry.h"

cWashEntry::cWashEntry(RTC_TimeTypeDef startTime, uint8_t bayNumber)
{
	mStartTime = startTime;
	mBayNumber = bayNumber;
}

cWashEntry::~cWashEntry()
{

}

void cWashEntry::end(RTC_TimeTypeDef endTime)
{
	mEndTime = endTime;
}
