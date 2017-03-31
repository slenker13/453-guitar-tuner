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

#endif /* MOTORCONTROL_H_ */
