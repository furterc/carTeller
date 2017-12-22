/*
 * ultra_s_sensor.h
 *
 *  Created on: 17 Dec 2017
 *      Author: christo
 */

#ifndef ULTRA_S_SENSOR_H_
#define ULTRA_S_SENSOR_H_

#define DISTANCE_ECHO_TIMEOUT		2000		//startup time
#define DISTANCE_SAMPLE_DUTY		1000/64  	//16 samples a second
#define DISTANCE_MAX				400*58		//450cm * multiplier
#define DISTANCE_TIMEOUT			100			//ms

#include "timer_ic.h"
#include "output.h"

class cUltraSSensor
{
	typedef enum
	{
		DISTANCE_UNKNOWN,
		DISTANCE_TRIG,
		DISTANCE_WAIT_ECHO,
		DISTANCE_RECEIVE_SAMPLE,
		DISTANCE_WAIT
	}distanceStates_t;

	cTimerIc *mTimer;
	cOutput *mTrigger;
	uint32_t mChannel;
	uint8_t mDebug;
	bool mBusy;

	distanceStates_t mState;
	uint32_t mLastSample;

	void pulse();

public:
	cUltraSSensor(cTimerIc *timer, cOutput *trigger, uint32_t channel);
	virtual ~cUltraSSensor();

	void sample();
	uint32_t getLastSample();
	void setDebug(uint8_t debug);
	bool run();
};

#endif /* ULTRA_S_SENSOR_H_ */
