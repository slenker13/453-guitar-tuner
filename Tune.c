/*
 * Tune.c
 *
 *  Created on: Apr 24, 2017
 *      Author: Steven Lenker
 */

#include "Tune.h"
#include "ADCInit.h"
#include "MotorControl.h"

#include <string.h>
#include <math.h>

//
// Globals
//
StringInformation stringInfo;   // Current string infromation
bool tuning;                    // System currently tuning
bool locked;                    // PLL locked onto a frequency
uint16_t currString;            // Current string being tuned
bool low;                       // True if tuning bottom 3 strings (6,4,2)

//
// Tuning configurations
//
const StringInformation stringInfoStandard = { { 329.63f, 246.94f, 196.00f, 146.83f, 110.00f, 82.41f },
                                               { { {0.0842187695194713f, -0.277991011575716f, 0.393170781513554f, -0.277991011575716f, 0.0842187695194713f},
                                                   {1.0f, -3.28342934920395f, 4.10065718538671f, -2.30170922061515f, 0.490107681833460f} },
                                                 { {0.0848489536797885f, -0.299815127730697f, 0.432380885579188f, -0.299815127730697f, 0.0848489536797885f},
                                                   {1.0f, -3.42761437674422f, 4.44410782570644f, -2.57970659651789f, 0.565661685033033f} },
                                                 { {0.0865418988813147f, -0.322674297357123f, 0.473089752635043f, -0.322674297357123f, 0.0865418988813147f},
                                                   {1.0f, -3.57137561251296f, 4.80547543876379f, -2.88582263520491f, 0.652547764637509f} },
                                                 { {0.0884528214178380f, -0.339432101138738f, 0.502256778967632f, -0.339432101138738f, 0.0884528214178380f},
                                                   {1.0f, -3.67175993929242f, 5.06925756738008f, -3.11822407134857f, 0.721024662786746f} },
                                                 { {0.0906289374970894f, -0.354367371392571f, 0.527569482510698f, -0.354367371392571f, 0.0906289374970894f},
                                                   {1.0f, -3.75763579243177f, 5.30253871813214f, -3.33012840165845f, 0.785318090677812f} },
                                                 { {0.0938700098820215f, -0.372530880434458f, 0.557333368616354f,  -0.372530880434458f, 0.0938700098820215f},
                                                   {1.0f, -3.85761510247108f, 5.58315992835721f, -3.59305483001974f, 0.867521631645089f} }
                                               } };


//
// init function
//
void initTune(void) {
    tuning = false;
    locked = false;
    low = true;
    currString = 6;

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
    float f0 = stringInfo.freqs[string - 1];

    // Filter buffers
    static float filtOutputs[5] = {0};
    static float filtInputs[5] = {0};

    // Loop variables
    static float freqIncr = 0.0f;      // The current frequency change
    static float freq = 0.0f;            // Current frequency of the follow signal
    // Preallocate phase vectors
    float phase1_old = 0.0f;
    float phase2_old = 0.0f;
    static float phase1 = 0.0f;
    static float phase2 = 0.0f;
    static uint32_t j = 0;               // Count num time samples
    // Will determine locked when abs(20 unit average for phase 2) < 0.5
    uint16_t avgLength = 7;
    static float avg[7] = {20.0f,20.0f,20.0f,20.0f,20.0f,20.0f,20.0f};
    static float avg2[7] = {20.0f,20.0f,20.0f,20.0f,20.0f,20.0f,20.0f};
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
        memcpy(avg, (float[7]){20.0f,20.0f,20.0f,20.0f,20.0f,20.0f,20.0f}, sizeof(float) * avgLength);
        memcpy(avg2, (float[7]){20.0f,20.0f,20.0f,20.0f,20.0f,20.0f,20.0f}, sizeof(float) * avgLength);
        memcpy(filtInputs, (float[5]){0.0f,0.0f,0.0f,0.0f,0.0f}, sizeof(float) * 5);
        memcpy(filtOutputs, (float[5]){0.0f,0.0f,0.0f,0.0f,0.0f}, sizeof(float) * 5);
        ptr = 0;
    }

    // LOOP
    // Load current input
    filtInputs[0] = adcIn;

    // Calculate current output
    filtOutputs[0] = stringInfo.coefs[string-1][0][0] * filtInputs[0] +
            stringInfo.coefs[string-1][0][1] * filtInputs[1] +
            stringInfo.coefs[string-1][0][2] * filtInputs[2] +
            stringInfo.coefs[string-1][0][3] * filtInputs[3] +
            stringInfo.coefs[string-1][0][4] * filtInputs[4] -
            stringInfo.coefs[string-1][1][1] * filtInputs[1] -
            stringInfo.coefs[string-1][1][2] * filtInputs[2] -
            stringInfo.coefs[string-1][1][3] * filtInputs[3] -
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

    phase1 = phase1_old - mu1 * filtOutputs[0] * sin(2 * PI * freq * j * (1.0f/(float)Fs) + phase1_old);
    phase2 = phase2_old - mu2 * filtOutputs[0] * sin(2 * PI * freq * j * (1.0f/(float)Fs) + phase1_old + phase2_old);

    // Calculate frequency adjustment
    freqIncr = freqIncr + (phase1 - phase1_old) / (float)Fs;

    // Every .01 seconds add a value to the averages
    if (fmod(j, 0.01 * Fs) == 0) {
        avg[ptr] = phase2;
        avg2[ptr] = phase1;
        ptr = (ptr + 1) % avgLength;
    }

    // Square eucledian norm is less than 0.1
    // Calculate averages
    float avg_1 = 0;
    float avg_2 = 0;
    uint16_t i;
    for (i = 0; i < avgLength; i++) {
        avg_1 += pow(avg[i], 2);
        avg_2 += pow(avg2[i], 2);
    }
    avg_1 = avg_1 / (float)avgLength;
    avg_2 = avg_2 / (float)avgLength;

    if (avg_1 < 0.1f && avg_2 < 0.1f) {
        locked = true;
    }

    // Adjust the frequency
    freq = freq + freqIncr;

    // reset counter;
    j++;

    // Tune the guitar!
    tuneGuitar(freq);
}

// Tune guitar
void tuneGuitar(float freq) {
    float diff;
    uint16_t dir = 0;

    // Get frequency difference
    diff = freq - stringInfo.freqs[currString - 1];

    // Check if in tune
    if ((diff < TUNING_THRESHOLD) && locked) {
        // In tune!!!!
        disableAllMotors();
        locked = false;

        return;
    }
    else if (diff < 0) {
        // String is flat
        dir = 1;
    }
    else {
        // String is sharp
        dir = 0;
    }

    // Turn Motor
    enableMotor(4, dir);
/*
    if (low) {
        enableMotor(currString, dir);
    }
    else {
        enableMotor(currString + 1, dir);
    }
*/
}
