/*
 * cRTC.h
 *
 *  Created on: 12 Jan 2018
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_CRTC_H_
#define DRIVERS_BSP_UTILS_CRTC_H_

#include "stm32l0xx_hal.h"
#include "terminal.h"

#define N_PREDIV_S                 10 /* subsecond number of bits */
#define PREDIV_S                  ((1<<N_PREDIV_S)-1) /* Synchonuous prediv  */
#define PREDIV_A                  (1<<(15-N_PREDIV_S))-1/* Asynchonuous prediv   */

class cRTC
{
	RTC_HandleTypeDef RtcHandle;
public:
	cRTC();
	virtual ~cRTC();
	static cRTC *getInstance();
	void init();

	void setTime(RTC_TimeTypeDef time);
	RTC_TimeTypeDef getTime();

	void setDate(RTC_DateTypeDef date);
	RTC_DateTypeDef getDate();
	};

#endif /* DRIVERS_BSP_UTILS_CRTC_H_ */
