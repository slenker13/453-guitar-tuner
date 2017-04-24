/*
 * MotorControl.c
 *
 *  Created on: Mar 31, 2017
 *      Author: Steven Lenker
 */

#include "MotorControl.h"


//
// Globals
//
uint16_t timeBasePeriod;
uint16_t stepCnt;


//
// init function
//
void initMotorController(uint16_t TBPRD) {
    // Set variables
    timeBasePeriod = TBPRD;
    stepCnt = 0;

    // Set up ISR
    Interrupt_register(INT_EPWM2, &epwm2ISR);

    // init PWM
    initPWM();

    // init other GPIO pins
    initPins();
}

//
// PWM init function
//
void initPWM() {
    //
    // Configure GPIO2 as ePWM2A
    //
    GPIO_setPadConfig(2, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_2_EPWM2A);

    //
    // Disable sync(Freeze clock to PWM as well)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Set-up TBCLK
    // CLOCK DIVIDER = 64
    //
    EPWM_setTimeBasePeriod(EPWM2_BASE, timeBasePeriod);
    EPWM_setPhaseShift(EPWM2_BASE, 0U);
    EPWM_setTimeBaseCounter(EPWM2_BASE, 0U);

    //
    // Set Compare values
    //
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A, timeBasePeriod);

    //
    // Set-up counter mode
    //
    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(EPWM2_BASE);
    EPWM_setClockPrescaler(EPWM2_BASE, EPWM_CLOCK_DIVIDER_64, EPWM_HSCLOCK_DIVIDER_1);

    //
    // Set Action qualifier
    //
    EPWM_setActionQualifierAction(EPWM2_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM2_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    //
    // Interrupt on rising edge
    //
    EPWM_setInterruptSource(EPWM2_BASE, EPWM_INT_TBCTR_D_CMPA);
    EPWM_enableInterrupt(EPWM2_BASE);
    EPWM_setInterruptEventCount(EPWM2_BASE, 1U);

    //
    // Enable sync and clock to PWM
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Enable interrupt
    //
    Interrupt_enable(INT_EPWM2);
}

//
// Init GPIO pins
//
void initPins() {
    //
    // Motor 1
    //
    GPIO_setPadConfig(13U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(13U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(14U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(14U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(15U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(15U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(16U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(16U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(13U, 1);  // DIR
    GPIO_writePin(14U, 0);  // SETMODE0
    GPIO_writePin(15U, 0);  // SETMODE1
    GPIO_writePin(16U, 1);  // ENABLE_N

    //
    // Motor 2
    //
    GPIO_setPadConfig(17U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(17U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(18U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(18U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(19U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(19U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(20U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(20U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(17U, 1);  // DIR
    GPIO_writePin(18U, 0);  // SETMODE0
    GPIO_writePin(19U, 0);  // SETMODE1
    GPIO_writePin(20U, 1);  // ENABLE_N

    //
    // Motor 3
    //
    GPIO_setPadConfig(21U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(21U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(41U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(41U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(42U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(42U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(43U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(43U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(21U, 1);  // DIR
    GPIO_writePin(41U, 0);  // SETMODE0
    GPIO_writePin(42U, 0);  // SETMODE1
    GPIO_writePin(43U, 1);  // ENABLE_N

    //
    // Motor 4
    //
    GPIO_setPadConfig(61U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(61U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(62U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(62U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(63U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(63U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(64U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(64U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(61U, 1);  // DIR
    GPIO_writePin(62U, 0);  // SETMODE0
    GPIO_writePin(63U, 0);  // SETMODE1
    GPIO_writePin(64U, 1);  // ENABLE_N

    //
    // Motor 5
    //
    GPIO_setPadConfig(65U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(65U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(66U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(66U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(69U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(69U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(70U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(70U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(65U, 1);  // DIR
    GPIO_writePin(66U, 0);  // SETMODE0
    GPIO_writePin(69U, 0);  // SETMODE1
    GPIO_writePin(70U, 1);  // ENABLE_N

    //
    // Motor 6
    //
    GPIO_setPadConfig(71U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(71U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(72U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(72U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(73U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(73U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(78U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(78U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(71U, 1);  // DIR
    GPIO_writePin(72U, 0);  // SETMODE0
    GPIO_writePin(73U, 0);  // SETMODE1
    GPIO_writePin(78U, 1);  // ENABLE_N

    // Miscellaneous pins
    GPIO_setPadConfig(3U, GPIO_PIN_TYPE_STD);   // DECAY
    GPIO_setDirectionMode(3U, GPIO_DIR_MODE_OUT);
    GPIO_writePin(3U, 0);

    GPIO_setPadConfig(4U, GPIO_PIN_TYPE_STD);   // RST_N
    GPIO_setDirectionMode(4U, GPIO_DIR_MODE_OUT);
    GPIO_writePin(4U, 1);
}

//
// epwm2ISR - ePWM 2 ISR
//
__interrupt void epwm2ISR(void) {
    //
    // Increment counter
    //
    stepCnt++;

    //
    // Clear INT flag for this timer
    //
    EPWM_clearEventTriggerInterruptFlag(EPWM2_BASE);

    //
    // Acknowledge interrupt group
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

//
// Turn motor specified number of degrees
//
void turnMotor(uint16_t motor, uint16_t direction, uint16_t degrees) {
    // Calculate steps
    uint16_t steps = degrees / 1.8;

    // Reset stepCnt
    stepCnt = 0;

    // Enable Motor and set direction
    if (motor == 1) {
        GPIO_writePin(13U, direction);
        GPIO_writePin(16U, 0);
    }
    else if (motor == 2) {
        GPIO_writePin(17U, direction);
        GPIO_writePin(20U, 0);
    }
    else if (motor == 3) {
        GPIO_writePin(21U, direction);
        GPIO_writePin(43U, 0);
    }
    else if (motor == 4) {
        GPIO_writePin(61U, direction);
        GPIO_writePin(64U, 0);
    }
    else if (motor == 5) {
        GPIO_writePin(65U, direction);
        GPIO_writePin(70U, 0);
    }
    else if (motor == 6) {
        GPIO_writePin(71U, direction);
        GPIO_writePin(78U, 0);
    }
    else {
        return;
    }

    // Wait for correct stepCnt
    while (stepCnt < steps) {
        NOP;
    }

    // Disable Motor
    if (motor == 1) {
        GPIO_writePin(16U, 1);
    }
    else if (motor == 2) {
        GPIO_writePin(20U, 1);
    }
    else if (motor == 3) {
        GPIO_writePin(43U, 1);
    }
    else if (motor == 4) {
        GPIO_writePin(64U, 1);
    }
    else if (motor == 5) {
        GPIO_writePin(70U, 1);
    }
    else if (motor == 6) {
        GPIO_writePin(78U, 1);
    }
}
