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

typedef struct
{
	uint8_t bayNumber;
	uint8_t date_dayOfMonth;
	uint8_t date_monthOfYear;
	uint8_t date_year;

	uint8_t time_hour;
	uint8_t time_minute;
	uint8_t time_second;
	uint8_t duration_minute;

	uint32_t placekeeper;

	uint8_t duration_second;
	uint8_t entry_set;
	uint8_t ack;
	uint8_t crc;

} sCarwashObject_t;

class cCarWash
{
	sCarwashObject_t mCarWashObj;
public:
	cCarWash(uint8_t bayNumber, uint8_t day, uint8_t month, uint8_t year);
	virtual ~cCarWash();

	static HAL_StatusTypeDef checkCrc(sCarwashObject_t *obj);

	void start(uint8_t hour, uint8_t minute, uint8_t second);
	void end(uint8_t hour, uint8_t minute, uint8_t second);
	void dbgPrint();
	static void dbgPrintObj(sCarwashObject_t obj);
	bool getObject(sCarwashObject_t *obj);
	uint8_t getBytes(uint8_t *data);
};

#endif /* CAR_WASH_H_ */
