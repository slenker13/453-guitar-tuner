/*
 * Bluetooth.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Steven Lenker
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Defines
//


/*********************/
/***** FUNCTIONS *****/
/*********************/

// init functions
void initBluetooth(void);
void initSCI(void);
void initGPIO(void);

// ISR
__interrupt void sciaRXFIFOISR(void);

#endif /* BLUETOOTH_H_ */
