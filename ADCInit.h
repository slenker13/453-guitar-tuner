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
#define RESULTS_BUFFER_SIZE  256
#define EX_ADC_RESOLUTION    ADC_RESOLUTION_12BIT   // Or ADC_RESOLUTION_12BIT
#define EX_ADC_SIGNAL_MODE   ADC_MODE_SINGLE_ENDED  // Or ADC_MODE_DIFFERENTIAL


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
// fill adc buffer
//
void fillADCBuffer();

//
// ADC ISR
//
__interrupt void adcA1ISR(void);

#endif /* ADCINIT_H_ */
