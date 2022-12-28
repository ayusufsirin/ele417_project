#include "uart.h"

#define RXD BIT1
#define TXD BIT2

volatile unsigned int uartNewLineFlag = 0;
volatile unsigned int uartNewLineCount = 0;
volatile unsigned int uartReadCount = 0;
volatile unsigned char uartReadBuffer[UART_RX_BUFFER_SIZE];
volatile unsigned char uartNewLineIndexes[NEW_LINE_INDEX_BUFFER_SIZE];

void SerialBegin()
{
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1SEL = TXD | RXD;
    P1SEL2 = TXD | RXD;

    UCA0CTL1 |= UCSWRST + UCSSEL_2;
    //settings for 19200 baud
    // UCA0BR0 = 52;
    // UCA0BR1 = 0;
    // UCA0MCTL = UCBRS_0;

    // settings for 9600 baud
    UCA0BR0 = 0x68;
    UCA0BR1 = 0x00;
    UCA0MCTL = UCBRS_2;

    UCA0CTL1 &= ~UCSWRST;

    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    _enable_interrupts();
}

void SerialWrite(unsigned char Byte)
{
    while ((IFG2 & UCA0TXIFG) == 0)
        ;
    UCA0TXBUF = Byte;
}

unsigned int SerialAvailable(void)
{
    if (uartNewLineCount == NEW_LINE_INDEX_BUFFER_SIZE)
        return 1;
    return 0;
}

unsigned char* SerialRead(void)
{
    return uartReadBuffer;
}

void SerialPrint(unsigned char *sPtr)
{
    for (; *sPtr != '\0'; sPtr++)
    {
        SerialWrite(*sPtr);
    }
}

void SerialFlush(void)
{
    unsigned char i;
    uartReadCount = 0;
    uartNewLineFlag = 0;
    uartNewLineCount = 0;

    for (i = NEW_LINE_INDEX_BUFFER_SIZE; i == 0; i--)
        uartNewLineIndexes[i] = CHAR_NULL;

    for (i = UART_RX_BUFFER_SIZE; i == 0; i--)
        uartReadBuffer[i] = CHAR_NULL;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void uartISR(void)
{
//    if (UCA0RXBUF == '$' || UCA0RXBUF == '!' || uartReadCount == UART_RX_BUFFER_SIZE) {
//        unsigned char i;
//
//        for (i = 0; i < uartReadCount; i++) {
//            SerialWrite(uartReadBuffer[i]);
//        }
//        SerialPrint("\r\nHEBELE\r\n");
//        SerialFlush();
//    }

//    uartReadBuffer[uartReadCount++] = UCA0RXBUF;
    SerialWrite(UCA0RXBUF);

//    if (UCA0RXBUF == LF)
//    {
//        uartNewLineIndexes[uartNewLineCount] = uartReadCount;
//        uartNewLineCount++;
//        uartNewLineFlag = 1;
//    }
}
