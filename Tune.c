/*
 * Tune.c
 *
 *  Created on: Apr 24, 2017
 *      Author: Steven Lenker
 */

#include "Tune.h"
#include "ADCInit.h"

#include <string.h>
#include <math.h>

//
// Globals
//
StringInformation stringInfoStandard;
StringInformation stringInfoDropD;
StringInformation stringInfo;
bool tuning;
bool locked;
float filtOutputs[5] = {0};
float filtInputs[5] = {0};


//
// init function
//
void initTune(void) {
    tuning = false;
    locked = false;

    // Fill stringInfo structs
    // Standard
    stringInfoStandard.freqs[0] = 329.63f;
    stringInfoStandard.freqs[1] = 246.94f;
    stringInfoStandard.freqs[2] = 196.00f;
    stringInfoStandard.freqs[3] = 146.83f;
    stringInfoStandard.freqs[4] = 110.00f;
    stringInfoStandard.freqs[5] = 82.41f;
    stringInfoStandard.coefs[0][0][0] = 0.0933574403794781f;
    stringInfoStandard.coefs[0][0][1] = -0.369884656403621f;
    stringInfoStandard.coefs[0][0][2] = 0.553071339696652f;
    stringInfoStandard.coefs[0][0][3] = -0.369884656403621f;
    stringInfoStandard.coefs[0][0][4] = 0.0933574403794781f;
    stringInfoStandard.coefs[0][1][0] = 1.0f;
    stringInfoStandard.coefs[0][1][1] = -3.84334691565065f;
    stringInfoStandard.coefs[0][1][2] = 5.54251045758585f;
    stringInfoStandard.coefs[0][1][3] = -3.55442229683941f;
    stringInfoStandard.coefs[0][1][4] = 0.855275662552577f;

    // Drop D
    /*
    stringInfoDropD.freqs[0] = 329.63f;
    stringInfoDropD.freqs[1] = 246.94f;
    stringInfoDropD.freqs[2] = 196.00f;
    stringInfoDropD.freqs[3] = 146.83f;
    stringInfoDropD.freqs[4] = 110.00f;
    stringInfoDropD.freqs[5] = 73.43f;
*/

    // Default to standard
    stringInfo = stringInfoStandard;
}

//
// Set string information struct
//
void setStringInfo(char config) {
    if (config == 's') {
        stringInfo = stringInfoStandard;
    }
}

// PLL algorithm
void PLL(bool newString, uint16_t string, float adcIn) {
    // Sample size
    uint16_t Fs = 10000;

    // Phase step size
    float mu1 = 0.1f;
    float mu2 = 0.01f;

    // Target frequency
    float f0 = stringInfo.freqs[string-1];

    // Loop variables
    //static float freqIncrOld = 0.0f;   // The last frequency change
    static float freqIncr = 0.0f;      // The current frequency change
    static float freq = 0.0f;            // Current frequency of the follow signal
    // Preallocate phase vectors
    float phase1_old = 0.0f;
    float phase2_old = 0.0f;
    static float phase1 = 0.0f;
    static float phase2 = 0.0f;
    static uint16_t j = 0;               // Count num time samples
    // Will determine locked when abs(20 unit average for phase 2) < 0.5
    uint16_t avgLength = 7;
    static float avg[7] = {20,20,20,20,20,20,20};
    static float avg2[7] = {20,20,20,20,20,20,20};
    static uint16_t ptr = 0;

    // Reset variables
    if (newString) {
        locked = false;
        freq = f0;
        //freqIncrOld = 0.0f;
        freqIncr = 0.0f;
        phase1 = 0.0f;
        phase2 = 0.0f;
        j = 0;
        memset(avg, 20, sizeof(float) * avgLength);
        memset(avg2, 20, sizeof(float) * avgLength);
        ptr = 0;
    }

    // Calculate current output
    filtOutputs[0] = stringInfo.coefs[string-1][0][0] * filtInputs[0] + \
            stringInfo.coefs[string-1][0][1] * filtInputs[1] + \
            stringInfo.coefs[string-1][0][2] * filtInputs[2] + \
            stringInfo.coefs[string-1][0][3] * filtInputs[3] + \
            stringInfo.coefs[string-1][0][4] * filtInputs[4] + \
            stringInfo.coefs[string-1][1][1] * filtInputs[1] + \
            stringInfo.coefs[string-1][1][2] * filtInputs[2] + \
            stringInfo.coefs[string-1][1][3] * filtInputs[3] + \
            stringInfo.coefs[string-1][1][4] * filtInputs[4];

    // Shift inputs
    filtInputs[4] = filtInputs[3];
    filtInputs[3] = filtInputs[2];
    filtInputs[2] = filtInputs[1];
    filtInputs[1] = filtInputs[0];

    // Shift outputs
    filtOutputs[4] = filtOutputs[3];
    filtOutputs[3] = filtOutputs[2];
    filtOutputs[2] = filtOutputs[1];
    filtOutputs[1] = filtOutputs[0];

    //freqIncrOld = freqIncr;
    phase1_old = phase1;
    phase2_old = phase2;

    phase1 = phase1_old - mu1 * filtOutputs[0] * sin(2 * PI * freq * j * 1/Fs + phase1_old);
    phase2 = phase2_old - mu2 * filtOutputs[0] * sin(2 * PI * freq * j * 1/Fs + phase1_old + phase2_old);

    // Calculate frequency adjustment
    freqIncr = freqIncr + (phase1 - phase1_old) / Fs;

    // Every .01 seconds add a value to the averages
    if (fmod(j, 0.01 * Fs) == 0) {
        avg[ptr] = phase2;
        avg2[ptr] = phase1;
        ptr = ptr % avgLength;
    }

    // Square eucledian norm is less than 0.1
    // Calculate averages
    float avg_1 = 0;
    float avg_2 = 0;
    uint16_t i;
    for (i = 0; i < avgLength; i++) {
        avg_1 += pow(avg[i], 2);
        avg_2 += pow(avg[i], 2);
    }
    avg_1 = avg_1 / avgLength;
    avg_2 = avg_2 / avgLength;

    if (avg_1 < 0.2f && avg_2 < 0.2f) {
        locked = true;
    }

    // Adjust the frequency
    freq = freq + freqIncr;

    // reset counter;
    j++;
}

// Tune guitar
void tuneGuitar(bool low, float s1, float s2, float s3) {
    float diff1, diff2, diff3;

    // Get frequency differences
    if (low) {
        // 2 4 6
        diff1 = s1 - stringInfo.freqs[1];
        diff2 = s2 - stringInfo.freqs[3];
        diff3 = s3 - stringInfo.freqs[5];
    }
    else {
        // 1 3 5
        diff1 = s1 - stringInfo.freqs[0];
        diff2 = s2 - stringInfo.freqs[2];
        diff3 = s3 - stringInfo.freqs[4];
    }

    // Check if in tune
    if (diff1 < TUNING_THRESHOLD && diff2 < TUNING_THRESHOLD && diff3 < TUNING_THRESHOLD) {
        // In tune!!!!
        // Turn off ADC
        stopADC();

        // Clear interrupts
        // Clear SCI interrupt
        SCI_clearOverflowStatus(SCIA_BASE);
        SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
        Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
        // Clear ADC interrupt
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
        Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

        return;
    }

    // String 1
    if (diff1 > TUNING_THRESHOLD) {
        // Turn on motor
    }
    else {
        // Turn off motor
    }

    // String 2
    if (diff2 > TUNING_THRESHOLD) {
        // Turn on motor
    }
    else {
        // Turn off motor
    }

    // String 3
    if (diff3 > TUNING_THRESHOLD) {
        // Turn on motor
    }
    else {
        // Turn off motor
    }

    // Clear ADC interrupt
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
