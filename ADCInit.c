/*
 * ADCInit.c
 *
 *  Created on: Apr 17, 2017
 *      Author: Steven Lenker
 */

#include "ADCInit.h"
#include "Tune.h"
#include "MotorControl.h"

#include <stdbool.h>

//
// Globals
//
bool newVal;
float adcResult;

//
// Init function
//
void initADC(void) {
    newVal = false;

    //
    // Map the ISR
    //
    Interrupt_register(INT_ADCA1, &adcA1ISR);

    //
    // Set ADCCLK divider to /4
    //
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);

    //
    // Set resolution and signal mode (see #defines above) and load
    // corresponding trims.
    //
    ADC_setMode(ADCA_BASE, EX_ADC_RESOLUTION, EX_ADC_SIGNAL_MODE);

    //
    // Set pulse positions to late
    //
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);

    //
    // Power up the ADC and then delay for 1 ms
    //
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);

    // Initialize the PWM used for sampling
    initEPWM();

    // Configure SOC0
    initADCSOC();

    //
    // Enable ADC interrupt
    //
    Interrupt_enable(INT_ADCA1);
}

//
// Function to configure ADCA's SOC0 to be triggered by ePWM1.
//
void initADCSOC(void) {
    //
    // Configure SOC0 of ADCA to convert pin A2. The EPWM1SOCA signal will be
    // the trigger.
    //
    // For 12-bit resolution, a sampling window of 15 (75 ns at a 200MHz
    // SYSCLK rate) will be used.  For 16-bit resolution, a sampling window of
    // 64 (320 ns at a 200MHz SYSCLK rate) will be used.
    //
#if(EX_ADC_RESOLUTION == ADC_RESOLUTION_12BIT)
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN2, 15);
#elif(EX_ADC_RESOLUTION == ADC_RESOLUTION_16BIT)
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN2, 64);
#endif

    //
    // Set SOC0 to set the interrupt 1 flag. Enable the interrupt and make
    // sure its flag is cleared.
    //
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
}

// PWM trigger init function
// Motor controllers use ePWM2, so use ePWM1
void initEPWM(void) {
    //
    // Disable SOCA
    //
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);

    //
    // Configure the SOC to occur on the first up-count event
    //
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1);

    //
    // Set the compare A value to  and the period to
    //
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 1250);
    EPWM_setTimeBasePeriod(EPWM1_BASE, 2500);

    //
    // Freeze the counter
    //
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//
// Start ADC
//
void startADC(void) {
    //
    // Start ePWM1, enabling SOCA and putting the counter in up-count mode
    //
    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
}

//
// Stop ADC
//
void stopADC(void) {
    //
    // Stop ePWM1, disabling SOCA and freezing the counter
    //
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//
// ADC ISR
//
__interrupt void adcA1ISR(void) {
    // Get the latest result and center it at 0
    adcResult = (float)ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0) - ADC_BASELINE;

    // Normalize result and account for string amplitude
    if (currString == 3) {
        adcResult = adcResult / 1024.0f;
    }
    else if (currString == 2) {
        adcResult = adcResult / 800.0f;
    }
    else if (currString == 1) {
        adcResult = adcResult / 200.0f;
    }
    else {
        adcResult = adcResult / 2048.0f;
    }

    newVal = true;

    // Clear ADC interrupt
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
