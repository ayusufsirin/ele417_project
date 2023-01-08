#include <msp430.h>
#include <stdio.h>
#include <stdint.h>

#include "msprf24.h"
#include "nrf_userconfig.h"
#include "hd44780.h"
#include "uart.h"
#include "nmea_gps.h"
#include "utils.h"

#define UART_BAUD           9600

struct Message
{
    float icTemperature;  // internal msp430 thermistor
    struct GPS gps;  // external uart gps module
};

struct Frame *frame;
struct Message *msg;

unsigned char msgBuffer[sizeof(struct Message)];
uint8_t buf[BUF_SIZE];

char lcdStr1[17];
char lcdStr2[40];

void nrfInit(void);
void lcdInit(void);
void lcdStrFormat(struct Message *msg);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    serialBegin(UART_BAUD);
    lcdInit();
    nrfInit();

    if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state()))
    {
        flush_rx();
    }
    msprf24_activate_rx();

    hd44780_clear_screen();

    while (1)
    {
        lcdStrFormat(msg);
//        circular_shift_left(lcdStr2, 1);
        hd44780_write_string(lcdStr1, 1, 1, NO_CR_LF);
        hd44780_write_string(lcdStr2, 2, 1, NO_CR_LF);

        if (rf_irq & RF24_IRQ_FLAGGED)
        {
            msprf24_get_irq_reason();
        }

        if (rf_irq & RF24_IRQ_RX)
        {
            r_rx_payload(BUF_SIZE, buf);
            msprf24_irq_clear(RF24_IRQ_RX);

            frame = (struct Frame*) buf;

            if (frame->index < FRAME_NUMBER)
            {
                unsigned int i;
                for (i = 0; i < FRAME_DATA_LEN; i++)
                {
                    msgBuffer[frame->index * FRAME_DATA_LEN + i] =
                            frame->data[i];
                }

                msg = (struct Message*) msgBuffer;
            }
        }
    }
}

void nrfInit(void)
{
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    rf_crc = RF24_EN_CRC | RF24_CRCO;
    rf_addr_width = 5;
    rf_speed_power = RF24_SPEED_1MBPS | RF24_POWER_0DBM;
    rf_channel = 120;

    msprf24_init();
    msprf24_set_pipe_packetsize(0, BUF_SIZE);
    msprf24_open_pipe(0, 1);

    // Address setup
    uint8_t addr[5] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
    w_rx_addr(0, addr);
}

void lcdInit(void)
{
    P1DIR |= BIT0 | BIT3 | BIT4;
    P2DIR |= BIT3 | BIT4 | BIT5;

    TA0CTL = TASSEL_1 | MC_1 | ID_0 | TACLR;  // ACLK, Up-to-CCR mode
    TA0CCR0 = 500;
    TA0CCTL0 = CCIE;  // Compare interrupt enable

    __enable_interrupt();
}

void lcdStrFormat(struct Message *msg)
{
    char date[9];
    char time[7];
    char latitude[12];
    char longitude[12];

    date_string(msg->gps.date, date);
    time_string(msg->gps.time, time);
    lat_long_string(msg->gps.latitude, latitude);
    lat_long_string(msg->gps.longitude, longitude);

    sprintf(lcdStr1, "%s   %s", date, time);
    sprintf(lcdStr2, "%s%c%s%c", latitude, msg->gps.n_s, longitude,
            msg->gps.e_w);
}

// Timer_A interrupt service routine for TA0CCR0
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
{
    hd44780_timer_isr();  // Call HD44780 state machine
    TA0CCTL0 &= ~CCIFG;  // Clear the interrupt flag
}
