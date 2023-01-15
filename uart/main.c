/*
 * uart.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#include <msp430.h>
#include "include/uart.h"

void serialBegin(int baud)
{
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1SEL = TXD | RXD;
    P1SEL2 = TXD | RXD;

    UCA0CTL1 |= UCSWRST + UCSSEL_2;
    UCA0BR0 = 104;  // (int) 1000000 / baud;
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS_2;
    UCA0CTL1 &= ~UCSWRST;

    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    _enable_interrupts();
}

void serialWrite(char byte)
{
    UCA0TXBUF = byte;
}

int serialAvailable(void)
{
    if ((IFG2 & UCA0TXIFG) != 0)
        return 1;
    return 0;
}

void serialPrint(char *str)
{
    while (*str != '\0')
    {
        while (serialAvailable() == 0)
            ;
        serialWrite(*str++);
    }
}
