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
    initMotorController(5000);
    initBluetooth();
    initADC();
    initTune();

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
        NOP;
    }
}

//
// End of File
//
