#include <stdio.h>

#include <msp430.h>
#include <hd44780.h>
#include <nrf24l01.h>
#include <uart.h>
#include <nmea_gps.h>

#define LCD_DB_MASK     (BIT4 + BIT5 + BIT6 + BIT7)  // P1.4 (D4) to P1.7 (D7)
#define LCD_CTL_MASK    (BIT0 + BIT3)                // P1.6 (E) and P1.7 (RS)

#define FRAME_DATA_LEN      TX_BUF_LEN - sizeof(unsigned char)
#define FRAME_NUMBER        3

struct Message
{
    float icTemperature;  // internal msp430 thermistor
    struct GPS gps;  // external uart gps module
};

struct Frame
{
    unsigned char index;  // 0-255 frame numbers
    char data[FRAME_DATA_LEN];
};

struct Frame *frame;

struct Message *msg;
unsigned char msgBuffer[sizeof(struct Message)];
unsigned msgBufferIndex = 0;

char lcdStr[32];

void initLCD(void);
void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    serialBegin(9600);
    nrfBeginRX();

    initLCD();
    hd44780_clear_screen();

    unsigned char *payload;

    while (1)
    {
        sprintf(lcdStr, "D:%dT%d La:%d%c Lo:%d%c", (int) msg->gps.date,
                (int) msg->gps.time, (int) msg->gps.latitude, msg->gps.n_s,
                (int) msg->gps.longitude, msg->gps.e_w);
        hd44780_write_string(lcdStr, 1, 1, CR_LF);

        payload = nrfReceive();
        if (nrfAvailable() > 0)
        {
            frame = (struct Frame*) (payload + TX_BUF_LEN);

            unsigned int j;
            for (j = 0; j < FRAME_DATA_LEN; j++)
            {
                msgBuffer[frame->index * FRAME_DATA_LEN + j] = frame->data[j];
            }

            msg = (struct Message*) msgBuffer;

            unsigned int i;
            for (i = TX_BUF_LEN; i < RX_BUF_LEN; i++)
            {
                while (serialAvailable() == 0)
                    ;
                serialWrite(payload[i]);
            }
        }
    }
}

void initLCD(void)
{
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P1DIR |= LCD_DB_MASK;
    P1DIR |= LCD_CTL_MASK;

    TA0CCR1 = 5000;                   // Set CCR1 value for 32.678 ms interrupt
    TA0CCTL1 = CCIE;                   // Compare interrupt enable
    TA0CTL = TASSEL_2 | MC_2 | TACLR;  // SMCLK, Continuous mode

    _enable_interrupts();
}

void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex)
{
    unsigned int i;
    for (i = 0; i < (endIndex - startIndex); i++)
    {
        dest[i] = src[i + startIndex];
    }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)            // Timer 0 A1 interrupt service
{
    if (TA0IV == 2)  // Determine the interrupt source
    {
        hd44780_timer_isr();                       // Call HD44780 state machine
    }
}
