/*
 * distance_sensor.h
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#ifndef DISTANCE_SENSOR_H_
#define DISTANCE_SENSOR_H_

#include "output.h"

#define DISTANCE_ECHO_TIMEOUT		2000		//startup time
#define DISTANCE_SAMPLE_DUTY		1000/16  	//16 samples a second
#define DISTANCE_MAX				400*58		//450cm * multiplier
#define DISTANCE_TIMEOUT			100			//ms

class cDistanceSensor
{
	cOutput *mTrigger;
	uint16_t mTickStart;
	uint16_t mTickEnd;
	uint16_t mMaxDistance;

	uint16_t mLastSample;
	bool mDataAvailable;

	typedef enum
	{
		DISTANCE_UNKNOWN,
		DISTANCE_TRIG,
		DISTANCE_WAIT_ECHO,
		DISTANCE_RECEIVE_SAMPLE,
		DISTANCE_WAIT
	}distanceStates_t;

	distanceStates_t state;
	uint32_t mIcChannel;

public:
	cDistanceSensor(cOutput *trigger, uint32_t maxDistance, uint8_t sensorNumber);
	virtual ~cDistanceSensor();

	void pulse();

	void setStart(uint16_t start);
	void setEnd(uint16_t end);
	void setDataAvailable();
	uint32_t getIcChannel();

	void run();
	uint16_t getLastSample();
};

#endif /* DISTANCE_SENSOR_H_ */
