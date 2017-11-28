/*
 * carCheck.h
 *
 *  Created on: 27 Nov 2017
 *      Author: christo
 */

#ifndef INC_CARCHECK_H_
#define INC_CARCHECK_H_

typedef enum
{
	CARCHECK_STATE_UNKNOWN,
	CARCHECK_STATE_IDLE,
	CARCHECK_STATE_DETECT,
	CARCHECK_STATE_START,
	CARCHECK_STATE_INPROGRESS,
	CARCHECK_STATE_END
}carCheckState_t;

void car_check_Init();
void car_check_Run(int distance);


#endif /* INC_CARCHECK_H_ */
