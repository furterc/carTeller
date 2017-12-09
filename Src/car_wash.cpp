/*
 * car_wash.cpp
 *
 *  Created on: 08 Dec 2017
 *      Author: christo
 */

#include "car_wash.h"
#include <string.h>

cCarWash::cCarWash(uint8_t bayNumber, uint8_t day, uint8_t month, uint8_t year)
{
	mSecond = 0;
	mCarWashObj.bayNumber = bayNumber;
	mCarWashObj.date_dayOfMonth = day;
	mCarWashObj.date_monthOfYear = month;
	mCarWashObj.date_year = year;
}

cCarWash::~cCarWash()
{

}

void cCarWash::start(uint8_t hour, uint8_t minute, uint8_t second)
{
	mCarWashObj.time_hour  = hour;
	mCarWashObj.time_minute = minute;
	mSecond = second;
}

void cCarWash::end(uint8_t hour, uint8_t minute, uint8_t second)
{
	uint8_t durMinute;
	uint8_t durSecond;

	if (minute < mCarWashObj.time_minute)
		minute += 60;

	durMinute = minute - mCarWashObj.time_minute;

	if (second < mSecond)
		second += 60;

	durSecond = second - mSecond;

	mCarWashObj.duration_minute = durMinute;
	mCarWashObj.duration_second = durSecond;
}

uint8_t cCarWash::getBytes(uint8_t *data)
{
	memcpy(data, &mCarWashObj, 8);

	return sizeof(sCarwashObject_t);
}
