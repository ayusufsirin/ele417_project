#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <msp430.h> 
#include <nrf24l01.h>
#include <uart.h>
#include <nmea_gps.h>
#include <aes.h>

#define NRF_CH 3
#define UART_BAUD           9600
#define FRAME_DATA_LEN      TX_BUF_LEN - sizeof(unsigned char)
#define FRAME_NUMBER        3  // 1 + (sizeof(struct Message) - 1) / FRAME_DATA_LEN

#define MODULE_1_IP 0x10
#define MODULE_2_IP 0x20
#define MODULE_3_IP 0x30

struct Message
{
    float icTemperature;  // internal msp430 thermistor
    struct GPS gps;  // external uart gps module
};

struct Frame
{
    unsigned char index;  // HN: Address, LN: Frame number
    char data[FRAME_DATA_LEN];
};

char gpsBuffer[NMEA_SENT_LEN];
unsigned int gpsBufferIndex = 0;

struct Frame frame;
struct Message msg;
char *msgBuffer;
char payload[] = "qwertyuiopasdfg";

// Array manipulation functions
void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    struct AES_ctx ctx;
    uint8_t key[] = "PraiseBeToAllah";
    uint8_t iv[] = "TheMostMerciful";

    AES_init_ctx_iv(&ctx, key, iv);

    serialBegin(UART_BAUD);
    nrfBeginTX(NRF_CH);

    while (1)
    {
        msg.icTemperature = 2.9f;
        msgBuffer = (char*) &msg;

        unsigned char fi;  // frame index (4 bits)
        for (fi = 0; fi < FRAME_NUMBER; fi++)
        {
//            frame.index = (fi & 0x0F) | 0x20;
            frame.index = fi;
            ch_arr_cpy(frame.data, msgBuffer, fi * FRAME_DATA_LEN,
                       (fi + 1) * FRAME_DATA_LEN - 1);

            ch_arr_cpy(payload, (char*) &frame, 0, TX_BUF_LEN);

            AES_CBC_encrypt_buffer(&ctx, (uint8_t*) payload, TX_BUF_LEN);
            nrfSend(payload);
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
