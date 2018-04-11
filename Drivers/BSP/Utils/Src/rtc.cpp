/*
 * cRTC.cpp
 *
 *  Created on: 12 Jan 2018
 *      Author: christo
 */

#include "../Inc/rtc.h"

cRTC::cRTC()
{
	// TODO Auto-generated constructor stub

}

cRTC::~cRTC()
{
	// TODO Auto-generated destructor stub
}

cRTC *cRTC::getInstance()
{
	static cRTC instance;
	//return the singleton
	return &instance;
}

HAL_StatusTypeDef cRTC::checkTime()
{
   RTC_TimeTypeDef time = getTime();
   RTC_DateTypeDef date = getDate();
   if(time.Hours == 0 && date.Year == 0 && date.Date == 1 && date.Month == 1)
       return HAL_TIMEOUT;

   return HAL_OK;
}

HAL_StatusTypeDef cRTC::init()
{
	RtcHandle.Instance = RTC;

	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = PREDIV_A; /* RTC_ASYNCH_PREDIV; */
	RtcHandle.Init.SynchPrediv = PREDIV_S; /* RTC_SYNCH_PREDIV; */
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
	    return HAL_ERROR;

	return checkTime();
}

void cRTC::setTime(RTC_TimeTypeDef time)
{
	HAL_RTC_SetTime(&RtcHandle, &time, RTC_FORMAT_BIN);
}

RTC_TimeTypeDef cRTC::getTime()
{
	RTC_TimeTypeDef time;
	HAL_RTC_GetTime(&RtcHandle, &time, RTC_FORMAT_BIN);
	return time;
}

void cRTC::setDate(RTC_DateTypeDef date)
{
	HAL_RTC_SetDate(&RtcHandle, &date, RTC_FORMAT_BIN);
}

RTC_DateTypeDef cRTC::getDate()
{
	RTC_DateTypeDef date;
	HAL_RTC_GetDate(&RtcHandle, &date, RTC_FORMAT_BIN);
	return date;
}

