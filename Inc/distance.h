/*
 * distance.h
 *
 *  Created on: 25 Nov 2017
 *      Author: christo
 */

#ifndef INC_DISTANCE_H_
#define INC_DISTANCE_H_

typedef enum
{
	DISTANCE_UNKNOWN,
	DISTANCE_STARTUP,
	DISTANCE_START_SAMPLE,
	DISTANCE_SAMPLE,
	DISTANCE_WAIT
}distanceStates_t;



void distance_Init();
void distance_IoInit();
void distance_IoDeInit();;
void distance_pulse();
void distance_timerIrq();
void distance_run();



#endif /* INC_DISTANCE_H_ */
