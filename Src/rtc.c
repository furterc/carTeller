/*
 * rtc.c
 *
 *  Created on: 11 Nov 2017
 *      Author: christo
 */

#include "rtc.h"

/* Private define ------------------------------------------------------------*/

/* MCU Wake Up Time */
#define MIN_ALARM_DELAY               3 /* in ticks */

/* subsecond number of bits */
#define N_PREDIV_S                 10

/* Synchonuous prediv  */
#define PREDIV_S                  ((1<<N_PREDIV_S)-1)

/* Asynchonuous prediv   */
#define PREDIV_A                  (1<<(15-N_PREDIV_S))-1

static RTC_HandleTypeDef RtcHandle =
{ 0 };

void rtc_init()
{
	RtcHandle.Instance = RTC;

	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = PREDIV_A; /* RTC_ASYNCH_PREDIV; */
	RtcHandle.Init.SynchPrediv = PREDIV_S; /* RTC_SYNCH_PREDIV; */
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	HAL_RTC_Init(&RtcHandle);

//	  /*Monday 1st January 2016*/
//	  RTC_DateStruct.Year = 0;
//	  RTC_DateStruct.Month = RTC_MONTH_JANUARY;
//	  RTC_DateStruct.Date = 1;
//	  RTC_DateStruct.WeekDay = RTC_WEEKDAY_MONDAY;
//	  HAL_RTC_SetDate(&RtcHandle , &RTC_DateStruct, RTC_FORMAT_BIN);
//
//	  /*at 0:0:0*/
//	  RTC_TimeStruct.Hours = 0;
//	  RTC_TimeStruct.Minutes = 0;
//
//	  RTC_TimeStruct.Seconds = 0;
//	  RTC_TimeStruct.TimeFormat = 0;
//	  RTC_TimeStruct.SubSeconds = 0;
//	  RTC_TimeStruct.StoreOperation = RTC_DAYLIGHTSAVING_NONE;
//	  RTC_TimeStruct.DayLightSaving = RTC_STOREOPERATION_RESET;
//
//	  HAL_RTC_SetTime(&RtcHandle , &RTC_TimeStruct, RTC_FORMAT_BIN);
}

void rtc_getTime(RTC_TimeTypeDef *time)
{
	HAL_RTC_GetTime(&RtcHandle, time, RTC_FORMAT_BIN);
}

void rtc_setTime(RTC_TimeTypeDef time)
{
	HAL_RTC_SetTime(&RtcHandle, &time, RTC_FORMAT_BIN);
}

void rtc_getDate(RTC_DateTypeDef *date)
{
	HAL_RTC_GetDate(&RtcHandle, date, RTC_FORMAT_BIN);
}

void rtc_setDate(RTC_DateTypeDef date)
{
	HAL_RTC_SetDate(&RtcHandle, &date, RTC_FORMAT_BIN);
}
