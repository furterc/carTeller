/*
 * distance.h
 *
 *  Created on: 25 Nov 2017
 *      Author: christo
 */

#ifndef INC_DISTANCE_H_
#define INC_DISTANCE_H_

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum
{
	DISTANCE_UNKNOWN,
	DISTANCE_TRIG,
	DISTANCE_WAIT_ECHO,
	DISTANCE_RECEIVE_SAMPLE,
	DISTANCE_WAIT
}distanceStates_t;



void distance_Init();
void distance_IoInit();
void distance_IoDeInit();;
void distance_pulse();
void distance_timerIrq();
void distance_run();
uint8_t distance_getLastSample(int *sample);

#ifdef __cplusplus
}
#endif

#endif /* INC_DISTANCE_H_ */
