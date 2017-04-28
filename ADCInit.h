/*
 * ADCInit.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Steven Lenker
 */

#ifndef ADCINIT_H_
#define ADCINIT_H_

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Defines
//
#define EX_ADC_RESOLUTION    ADC_RESOLUTION_12BIT   // Or ADC_RESOLUTION_12BIT
#define EX_ADC_SIGNAL_MODE   ADC_MODE_SINGLE_ENDED  // Or ADC_MODE_DIFFERENTIAL

#define ADC_BASELINE    2200
#define ADC_THRESHOLD    .05


/*********************/
/***** FUNCTIONS *****/
/*********************/

//
// Init ADC
//
void initADC(void);

//
// Function to configure ADCA's SOC0 to be triggered by ePWM1.
//
void initADCSOC(void);

//
// Init EPWM for trigger
//
void initEPWM(void);

//
// Start and Stop ADC
//
void startADC(void);
void stopADC(void);

//
// ADC ISR
//
__interrupt void adcA1ISR(void);

#endif /* ADCINIT_H_ */
