#include <stdio.h>
#include <stdlib.h>

#include <msp430.h>
#include <uart.h>

#define PIN_X BIT3  // P1.3
#define PIN_Y BIT4  // P1.4
#define PIN_Z BIT5  // P1.5

#define LED1 BIT0

int pin_x;
int pin_y;
int pin_z;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                         //Stop WDT
    BCSCTL1 = CALBC1_8MHZ;                                 //Set DCO to 8Mhz
    DCOCTL = CALDCO_8MHZ;                                //Set DCO to 8Mhz
    P1DIR &= ~0xF0;                                 // set pin direction for led
    P1DIR |= BIT0;
    P2DIR &= ~BIT3;

    __enable_interrupt();

    while (1)
    {
        ADC10CTL1 = INCH_3 + ADC10DIV_3;
        ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
        ADC10AE0 |= PIN_X;
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);         // LPM0 with interrupts enabled
        pin_x = ADC10MEM;
        __delay_cycles(100000); // Wait for ADC Ref to settle

        ADC10CTL1 = INCH_4 + ADC10DIV_3;
        ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
        ADC10AE0 |= PIN_Y;
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled
        pin_y = ADC10MEM;
        __delay_cycles(100000);

        ADC10CTL1 = INCH_5 + ADC10DIV_3;
        ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
        ADC10AE0 |= PIN_Z;
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled
        pin_z = ADC10MEM;
        __delay_cycles(100000);
    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Return to active mode
    P1OUT ^= LED1;
}
