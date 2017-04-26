/*
 * Bluetooth.c
 *
 *  Created on: Apr 17, 2017
 *      Author: Steven Lenker
 */

#include "Bluetooth.h"
#include "Tune.h"
#include "ADCInit.h"


//
// Globals
//
uint16_t sendChar;
uint16_t recievedChar;

//
// init function
//
void initBluetooth(void) {
    initSCI();
    initGPIO();
}

//
// Init SCI UART
//
void initSCI(void) {
    // RX pin (GPIO 85)
    GPIO_setMasterCore(85, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_85_SCIRXDA);
    GPIO_setDirectionMode(85, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(85, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(85, GPIO_QUAL_ASYNC);

    // TX pin (GPIO 84)
    GPIO_setMasterCore(84, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_84_SCITXDA);
    GPIO_setDirectionMode(84, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(84, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(84, GPIO_QUAL_ASYNC);

    // Map interrupts
    Interrupt_register(INT_SCIA_RX, sciaRXFIFOISR);

    // Initialize the SCI FIFO
    // 8 char bits, 1 stop bit, no parity. Baud rate is 115200.
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIA_BASE);
    SCI_disableLoopback(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);

    // RX FIFO Interrupt Enabled
    SCI_enableInterrupt(SCIA_BASE, (SCI_INT_RXFF));
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIA_BASE);

    SCI_resetTxFIFO(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);

    Interrupt_enable(INT_SCIA_RX);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//
// Init GPIO pins
//
void initGPIO(void) {
    GPIO_setPadConfig(86U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(86U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(87U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(87U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(89U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(89U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(90U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(90U, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(91U, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(91U, GPIO_DIR_MODE_OUT);

    GPIO_writePin(86U, 0);  // Auto-discovery (GPIO3)
    GPIO_writePin(87U, 0);  // Factory reset (GPIO4)
    GPIO_writePin(89U, 0);  // BT master (GPIO6)
    GPIO_writePin(90U, 0);  // Baud rate (GPIO7) (115k)
    GPIO_writePin(91U, 1);  // Reset_n
}

//
// RX ISR
//
__interrupt void sciaRXFIFOISR(void) {
    recievedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

    /**** HANDLE TUNING ****/
    // check char
    // Standard Tuning
    setStringInfo('s');

    // Turn on ADC
    startADC();
}
