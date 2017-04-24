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
    initBluetooth();
    initADC();

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // IDLE loop. Just sit and loop forever (optional):
    //

    //turnMotor(4, 0, 360);
    //turnMotor(2, 1, 180);
    //turnMotor(3, 0, 720);
    //turnMotor(4, 1, 360);

    //SCI_writeCharNonBlocking(SCIA_BASE, 'a');

    for(;;)
    {
        //NOP;
        //SCI_writeCharNonBlocking(SCIA_BASE, 'a');
        //DEVICE_DELAY_US(1000000);
        fillADCBuffer();
    }
}

//
// End of File
//
