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
                if (!tuned) {
                    if (!tuning) {
                        // Beginning of tuning
                        tuning = true;
                        PLL(true, currString, adcResult);
                    }
                    else {
                        // Already tuning
                        PLL(false, currString, adcResult);
                    }
                }
                thresholdCounter = 100;
            }
            else {
                if (tuning) {
                    PLL(false, currString, adcResult);
                }

                if (thresholdCounter > 0) {
                    thresholdCounter--;
                }
            }

            if (thresholdCounter == 0 && (tuning || tuned)) {
                //stopADC();
                tuning = false;

                if (tuned) {
                    tuned = false;
                    turnMotor(2, 1, 360.0f);

                    if (stage == 1) {
                        if (currString == 4) {
                            stage = 2;
                            currString = 5;
                        }
                        else {
                            currString = 4;
                        }
                    }
                    else if (stage == 2) {
                        if (currString == 3) {
                            stage = 3;
                            currString = 2;
                        }
                        else {
                            currString = 3;
                        }
                    }
                    else if (stage == 3) {
                        stage = 4;
                        currString = 1;
                    }
                    else if (stage == 4) {
                        stage = 5;
                    }

                    if (stage == 5) {
                        // DONE
                        stopADC();
                        break;
                    }
                }

                //startADC();
                // Send message to strum again
            }
        }

    }
}

//
// End of File
//
