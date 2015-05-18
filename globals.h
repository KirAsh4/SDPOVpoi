/*
 * globals.h
 *
 * Created: 5/12/2015 11:59:29 AM
 *  Author: Ashley M. Kirchner
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>
#include "Arduino.h"

extern const uint8_t      battPin          = A0;
extern const uint8_t      battReadings     = 20;
extern volatile uint16_t  battTot          = 0;
extern volatile uint16_t  battTop          = 0;
extern volatile uint16_t  battAvg          = 0;
extern volatile uint16_t  battVal          = 0;

#endif /* GLOBALS_H_ */