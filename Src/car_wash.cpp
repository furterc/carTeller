/*
 * car_wash.cpp
 *
 *  Created on: 08 Dec 2017
 *      Author: christo
 */

#include "car_wash.h"
#include "crc.h"
#include <string.h>

#include "../Drivers/BSP/Utils/Inc/rtc.h"
cCarWash::cCarWash(uint8_t bayNumber, uint8_t day, uint8_t month, uint8_t year)
{
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
	mCarWashObj.time_second = second;
}

void cCarWash::end(uint8_t hour, uint8_t minute, uint8_t second)
{
	uint8_t durMinute;
	uint8_t durSecond;

	if (minute < mCarWashObj.time_minute)
		minute += 60;

	durMinute = minute - mCarWashObj.time_minute;

	if (second < mCarWashObj.time_second)
		second += 60;

	durSecond = second - mCarWashObj.time_second;

	mCarWashObj.duration_minute = durMinute;
	mCarWashObj.duration_second = durSecond;
}

void cCarWash::dbgPrint()
{
	printf(GREEN_B("Bay : %d\n"), mCarWashObj.bayNumber);
	printf("Date: %d/%d/20%02d\n", mCarWashObj.date_dayOfMonth, mCarWashObj.date_monthOfYear, mCarWashObj.date_year);
	printf("Time: %02d:%02d\n", mCarWashObj.time_hour, mCarWashObj.time_minute);
	printf("Dur : %02d:%02d\n", mCarWashObj.duration_minute, mCarWashObj.duration_second);
}

void cCarWash::dbgPrintObj(sCarwashObject_t obj)
{
	printf(GREEN_B("Carwash bay: %d\n"), obj.bayNumber);
		printf("Date: %d/%d/20%02d\n", obj.date_dayOfMonth, obj.date_monthOfYear, obj.date_year);
		printf("Time: %02d:%02d\n", obj.time_hour, obj.time_minute);
		printf("Duration: %02d:%02d\n", obj.duration_minute, obj.duration_second);
}

bool cCarWash::getObject(sCarwashObject_t *obj)
{
	mCarWashObj.placekeeper = 0xFFFFFFFF;
	mCarWashObj.entry_set = 0x00;
	mCarWashObj.ack = 0xFF;

	*obj = mCarWashObj;
	return true;
}

HAL_StatusTypeDef cCarWash::checkCrc(sCarwashObject_t *obj)
{
	if (cCrc::crc8((uint8_t *) obj, sizeof(sCarwashObject_t)))
		return HAL_ERROR;

	return HAL_OK;
}

uint8_t cCarWash::getBytes(uint8_t *data)
{
	uint32_t size = sizeof(sCarwashObject_t);
	memcpy(data, &mCarWashObj, size);

	return size;
}
