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
extern bool tuning;

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
void tuneGuitar(bool low, float s1, float s2, float s3);

#endif /* TUNE_H_ */
