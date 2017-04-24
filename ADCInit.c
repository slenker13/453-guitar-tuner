/*
 * ADCInit.c
 *
 *  Created on: Apr 17, 2017
 *      Author: Steven Lenker
 */

#include "ADCInit.h"


//
// Globals
//
uint16_t adcAResults[RESULTS_BUFFER_SIZE];   // Buffer for results
uint16_t index;                              // Index into result buffer
volatile uint16_t bufferFull;                // Flag to indicate buffer is full

//
// Init function
//
void initADC(void) {
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
    // Initialize results buffer
    //
    for(index = 0; index < RESULTS_BUFFER_SIZE; index++)
    {
        adcAResults[index] = 0;
    }

    index = 0;
    bufferFull = 0;

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
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 12500);
    EPWM_setTimeBasePeriod(EPWM1_BASE, 25000);

    //
    // Freeze the counter
    //
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//
// fill adc buffer
//
void fillADCBuffer() {
    //
    // Start ePWM1, enabling SOCA and putting the counter in up-count mode
    //
    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);

    //
    // Wait while ePWM1 causes ADC conversions which then cause interrupts.
    // When the results buffer is filled, the bufferFull flag will be set.
    //
    while(bufferFull == 0)
    {
    }
    bufferFull = 0;     // Clear the buffer full flag

    //
    // Stop ePWM1, disabling SOCA and freezing the counter
    //
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

    //
    // Software breakpoint. At this point, conversion results are stored in
    // adcAResults.
    //
    // Hit run again to get updated conversions.
    //
    ESTOP0;
}

//
// ADC ISR
//
__interrupt void adcA1ISR(void) {
    //
    // Add the latest result to the buffer
    //
    adcAResults[index++] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

    //
    // Set the bufferFull flag if the buffer is full
    //
    if(RESULTS_BUFFER_SIZE <= index)
    {
        index = 0;
        bufferFull = 1;
    }

    //
    // Clear the interrupt flag and issue ACK
    //
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
