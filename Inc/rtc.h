/*
 * rtc.h
 *
 *  Created on: 11 Nov 2017
 *      Author: christo
 */

#ifndef SRC_RTC_H_
#define SRC_RTC_H_

#include "stm32l0xx_hal.h"
#include "stm32l0xx.h"


#ifdef __cplusplus
 extern "C" {
#endif

void rtc_init();
void rtc_getTime(RTC_TimeTypeDef *time);
void rtc_setTime(RTC_TimeTypeDef time);
void rtc_getDate(RTC_DateTypeDef *date);
void rtc_setDate(RTC_DateTypeDef date);

#ifdef __cplusplus
}
#endif

#endif /* SRC_RTC_H_ */
