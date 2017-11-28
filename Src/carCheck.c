/*
 * carCheck.c
 *
 *  Created on: 27 Nov 2017
 *      Author: christo
 */

#include <stdlib.h>
#include "carCheck.h"
#include "terminal.h"
#include "nvm.h"
#include "rtc.h"

carCheckState_t carState = CARCHECK_STATE_UNKNOWN;
RTC_TimeTypeDef startTime;
RTC_TimeTypeDef endTime;

int triggerDistance;
int triggerTime;

void car_check_Init()
{
	triggerDistance = *nvm_get_CarDistance();
	triggerTime = *nvm_get_CarTime();
	printf(GREEN("CARCHECK init\n"));
	printf(GREEN("dist: %d\n"), triggerDistance);
	printf(GREEN("time: %d\n"), triggerTime);
}


void car_check_Run(int distance)
{
	static uint8_t carCount = 0;
	switch (carState) {
		case CARCHECK_STATE_IDLE:
			carCount = 0;
			if (distance < triggerDistance)
			{
				rtc_getTime(&startTime);
				printf(YELLOW("Car detected @%02d:%02d:%02d\n"), startTime.Hours, startTime.Minutes, startTime.Seconds);
				carState = CARCHECK_STATE_DETECT;
			}

			break;
		case CARCHECK_STATE_DETECT:
			if (distance > triggerDistance)
			{
				printf(RED("car got away\n"));
				carState = CARCHECK_STATE_IDLE;
				return;
			}

			printf(GREEN("%d - %dcm\n"), carCount, distance);
			carCount++;

			if (carCount > triggerTime)
			{
				carState = CARCHECK_STATE_START;
				return;
			}
			break;
		case CARCHECK_STATE_START:
			printf(GREEN("Carwash started@%02d:%02d:%02d\n"), startTime.Hours, startTime.Minutes, startTime.Seconds);
			carState = CARCHECK_STATE_INPROGRESS;
			break;
		case CARCHECK_STATE_INPROGRESS:
			if (distance > triggerDistance)
			{
				carState = CARCHECK_STATE_END;
				return;
			}
			break;
		case CARCHECK_STATE_END:
		{
			rtc_getTime(&endTime);
			printf(CYAN_B("Carwash stats\n"));
			printf(CYAN("Start:\t%02d:%02d:%02d\n"), startTime.Hours, startTime.Minutes, startTime.Seconds);
			printf(CYAN("End:\t%02d:%02d:%02d\n"), endTime.Hours, endTime.Minutes, endTime.Seconds);
			carState = CARCHECK_STATE_IDLE;

		}
			break;
		default:
			carState = CARCHECK_STATE_IDLE;
			break;
	}
}

void carDistance(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
    	printf("trigDistance: %dcm\n", triggerDistance);
    }
    else if (argc == 2)
    {
    	uint8_t data = atoi(argv[1]);
    	if (nvm_set_CarDistance(&data) != 1)
    		printf("'n fokken gemors\n");

    	triggerDistance = data;
    	printf("trigDistance: %dcm\n", triggerDistance);
    }
}
sTermEntry_t carDistEntry =
{ "cd", "Set/Get the car trigger distance", carDistance };

void carTime(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
    	printf("trigTime: %dcm\n", triggerDistance);
    }
    else if (argc == 2)
    {
    	uint8_t data = atoi(argv[1]);
    	if (nvm_set_CarTime(&data) != 1)
    		printf("'n fokken gemors\n");

    	triggerTime = data;
    	printf("trigTime: %dcm\n", triggerTime);
    }
}
sTermEntry_t carTimeEntry =
{ "ct", "Set/Get the car trigger time", carTime };
