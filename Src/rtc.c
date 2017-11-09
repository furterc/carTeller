/*
 * rtc.c
 *
 *  Created on: 08 Nov 2017
 *      Author: christo
 */

#include "rtc.h"
#include "stm32l0xx_hal.h"
#include "terminal.h"
#include "commands.h"
#include <stdlib.h>

RTC_HandleTypeDef hrtc;

void rtc_init() {
	/**Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
//	__HAL_RCC_RTC_ENABLE();
}

void rtcSetGet(uint8_t argc, char **argv)
{
	RTC_TimeTypeDef time;
	if (HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
	{
		printf("getTimefailed\n");
	}
	if (argc == 1)
	{
		printf("Time: %d:%d:%d\n", time.Hours, time.Minutes, time.Seconds);
	}
	else if (argc == 3)
	{
		time.Hours = atoi(argv[1]);
		time.Minutes = atoi(argv[2]);
		time.Seconds = 0;

		if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK) {
			printf("Set Time: %d:%d:%d\n", time.Hours, time.Minutes, time.Seconds);
		}
	}
}
sTermEntry_t rtcEntry = { "t", "Set/Get the rtc", rtcSetGet };


void dateSetGet(uint8_t argc, char **argv)
{

	RTC_DateTypeDef date;
	if (HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK) {
		printf("getDatefailed\n");
	}
	if (argc == 1)
	{
		printf("Date: %d/%d/20%02d\n", date.Date, date.Month, date.Year);
	}
	else if (argc == 4)
	{
		date.Date = atoi(argv[1]);
		date.Month = atoi(argv[2]);
		date.Year = atoi(argv[3]);

		if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) == HAL_OK) {
			printf("Set Date: %d/%d/20%02d\n", date.Date, date.Month, date.Year);
		}
	}
}
sTermEntry_t dateEntry = { "d", "Set/Get the date", dateSetGet};
