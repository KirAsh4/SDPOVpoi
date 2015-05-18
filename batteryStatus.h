/*
 * batteryStatus.h
 *
 * Created: 5/11/2015 11:52:09 PM
 *  Author: Ashley M. Kirchner
 */ 


#ifndef BATTERYSTATUS_H_
#define BATTERYSTATUS_H_

#include <stdint.h>

static uint8_t batteryCheck() {
	// Battery Check - takes 2 seconds to take readings!!
	for (uint8_t c = 0; c < battReadings; c++) {
		battVal = analogRead(battPin);
		battTot = battTot + battVal;
		_delay_ms(100);
	}
	battAvg = battTot / battReadings;
}

#endif /* BATTERYSTATUS_H_ */