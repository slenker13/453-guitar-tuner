/*
 * Tune.h
 *
 *  Created on: Apr 24, 2017
 *      Author: Steven Lenker
 */

#ifndef TUNE_H_
#define TUNE_H_

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include <stdbool.h>

//
// Defines
//
#define TUNING_THRESHOLD 0.4f
#define PI 3.14159265358979323846

//
// Globals
//
extern bool tuning;                    // System currently tuning
extern bool locked;                    // PLL locked onto a frequency
extern uint16_t currString;            // Current string being tuned
extern bool low;                       // True if tuning bottom 3 strings (6,4,2)

//
// String frequencies struct
//
typedef struct StringInformationTag {
    float freqs[6];
    float coefs[6][2][5];
} StringInformation;

//StringInformation stringInfoDropD;


/*********************/
/***** FUNCTIONS *****/
/*********************/

// init function
void initTune(void);

// Set correct stringInfo
void setStringInfo(char config);

// Run PLL algorithm
void PLL(bool newString, uint16_t string, float adcIn);

// Tune guitar
void tuneGuitar(float freq);

#endif /* TUNE_H_ */
