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

	if (battAvg < 569) {
		return 0;
	}
	else {
		// 5V   = 1023   -> 204.60 per volt
		// 4.2V = 859.32 -> round to 859  ! with 10K series resistor, this reading drops to 814 !
		// 3.0V = 613.80 -> round to 614  ! with 10K series resistor, this reading drops to 569 !  <-- 3.0V safe cut-off
		// 2.8V = 572.88 -> round to 573  ! with 10K series resistor, this reading drops to 528 !
		battTop = map(battAvg, 569, 814, 0, 48);
		// Set PWR_ON pin high, turn strips on to display power level
		digitalWrite(STRIP_PWR, HIGH);
		_delay_ms(5);
		fill_rainbow(&(leds[0]), battTop, 0, 2);
		LEDS.setBrightness(16);
		LEDS.show();
		_delay_ms(SECOND);
		LEDS.setBrightness(64);
		resetString(1);
		// Set PWR_ON pin low, turn strips off
		// THIS IS CURRENTLY NOT WORKING, CHECK HARDWARE
		//_delay_ms(5);
		//digitalWrite(STRIP_PWR, LOW);
		return 1;
	}
}


#endif /* BATTERYSTATUS_H_ */