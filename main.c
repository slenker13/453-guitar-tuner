//#############################################################################
//
// FILE:   main.c
//
// TITLE:  Guitar Tuner Controller Software
//
// guitar_tuner_control
//
// main.c
//
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "MotorControl.h"
#include "Bluetooth.h"
#include "ADCInit.h"
#include "Tune.h"

#include <stdbool.h>

//
// Main
//
void main(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pull ups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    /************************/
    /***** INIT MODULES *****/
    /************************/
    initMotorController(2000);
    //initBluetooth();
    initADC();
    initTune();

    uint16_t thresholdCounter = 0;
    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // IDLE loop
    //
    startADC();
    for(;;)
    {
        if (newVal) {
            newVal = false;

            if (adcResult > ADC_THRESHOLD || adcResult < -ADC_THRESHOLD) {
                if (!tuning) {
                    // Beginning of tuning
                    tuning = true;
                    PLL(true, currString, adcResult);
                }
                else {
                    // Already tuning
                    PLL(false, currString, adcResult);
                }
                thresholdCounter = 100;
            }
            else {
                if (tuning) {
                    PLL(false, currString, adcResult);
                }
                thresholdCounter--;
            }

            if (thresholdCounter == 0 && tuning) {
                tuning = false;

                // If locked swtich to next string
                if (locked) {
                    locked = false;
                    currString += 2;
                }

                // Send message to strum again

            }
        }

    }
}

//
// End of File
//
