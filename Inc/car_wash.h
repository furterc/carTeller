/*
 * car_wash.h
 *
 *  Created on: 08 Dec 2017
 *      Author: christo
 */

#ifndef CAR_WASH_H_
#define CAR_WASH_H_

#include <stdint.h>

#include "terminal.h"

class cCarWash
{
	typedef struct
	{
		uint8_t bayNumber;
		uint8_t date_dayOfMonth;
		uint8_t date_monthOfYear;
		uint8_t date_year;

		uint8_t time_hour;
		uint8_t time_minute;
		uint8_t duration_minute;
		uint8_t duration_second;
	} sCarwashObject_t;

	sCarwashObject_t mCarWashObj;

	uint8_t mSecond;
public:
	cCarWash(uint8_t bayNumber, uint8_t day, uint8_t month, uint8_t year);
	virtual ~cCarWash();

	void start(uint8_t hour, uint8_t minute, uint8_t second);
	void end(uint8_t hour, uint8_t minute, uint8_t second);
	void dbgPrint();
	uint8_t getBytes(uint8_t *data);
};

#endif /* CAR_WASH_H_ */
