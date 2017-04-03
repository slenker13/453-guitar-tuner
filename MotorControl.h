/*
 * MotorControl.h
 *
 *  Created on: Mar 31, 2017
 *      Author: Steven Lenker
 */

#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

//
// Included files
//
#include "driverlib.h"
#include "device.h"
#include <stdint.h>
#include <stdbool.h>

//
// Defines
//
#define EPWM_CMP_UP           1U
#define EPWM_CMP_DOWN         0U


/*********************/
/***** FUNCTIONS *****/
/*********************/

// init function
void initMotorController(int TBPRD);

// init ePWM2
void initPWM();

// init other necessary pins
void initPins();

// ISR for counting pulses and determining position
__interrupt void epwm2ISR(void);

// Turn motor
void turnMotor(uint16_t motor, uint16_t direction, uint16_t degrees);

#endif /* MOTORCONTROL_H_ */
