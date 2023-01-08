/*
 * gy61.c
 *
 *  Created on: Jan 1, 2023
 *      Author: Ahmet Yusuf Sirin
 */

#include <msp430.h> 
#include "gy61.h"

void gy61Init(void){
    BCSCTL1 = CALBC1_8MHZ;  //Set DCO to 8Mhz
    DCOCTL = CALDCO_8MHZ;   //Set DCO to 8Mhz

    ADC10AE0 |= GY61_PIN_X + GY61_PIN_Y + GY61_PIN_Z;

    _enable_interrupts();
}

void gy61Read(int *x, int *y, int *z)
{
    ADC10CTL1 = INCH_3 + ADC10DIV_3;
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    *x = ADC10MEM;
    __delay_cycles(100000);                 // Wait for ADC Ref to settle

    ADC10CTL1 = INCH_4 + ADC10DIV_3;
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    *y = ADC10MEM;
    __delay_cycles(100000);

    ADC10CTL1 = INCH_5 + ADC10DIV_3;
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    *z = ADC10MEM;
    __delay_cycles(100000);
}
