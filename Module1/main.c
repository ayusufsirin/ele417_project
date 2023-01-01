#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <msp430.h> 
#include <nrf24l01.h>
#include <uart.h>
#include <nmea_gps.h>

#define UART_BAUD           9600
#define FRAME_DATA_LEN      TX_BUF_LEN - sizeof(unsigned char)
#define FRAME_NUMBER        3  // (int) ceil(((double) sizeof(struct Message)) / FRAME_DATA_LEN)

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

char gpsBuffer[NMEA_SENT_LEN] = "qwertyuiopasdfg";
unsigned int gpsBufferIndex = 0;

struct Message msg;
char *msgBuffer;
struct Frame frame;

// Array manipulation functions
void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    serialBegin(UART_BAUD);
    nrfBeginTX();

    while (1)
    {
        msg.icTemperature = 2.9f;
        msgBuffer = (char*) &msg;

        unsigned char fi;  // frame index
        for (fi = 0; fi < FRAME_NUMBER; fi++)
        {
            frame.index = fi;
            ch_arr_cpy(frame.data, msgBuffer, fi * FRAME_DATA_LEN,
                       (fi + 1) * FRAME_DATA_LEN - 1);
            nrfSend((char*) &frame);
            _delay_cycles(100000);
        }
    }
}

void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex)
{
    unsigned int i;
    for (i = 0; i < (endIndex - startIndex); i++)
    {
        dest[i] = src[i + startIndex];
    }
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void UART_RX_ISR(void)
{
    if (UCA0RXBUF == '$')
    {
        gpsBuffer[gpsBufferIndex] = '\0';

        if (memcmp(gpsBuffer, "$GPRMC", 6) == 0)  // check for GPS message_id
        {
            parse(&msg.gps, gpsBuffer);
        }
    }

    // To keep secure the buffer
    if (gpsBufferIndex >= NMEA_SENT_LEN || UCA0RXBUF == '$')
    {
        gpsBufferIndex = 0;
    }

    gpsBuffer[gpsBufferIndex++] = UCA0RXBUF;
}
