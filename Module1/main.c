#include <msp430.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "msprf24.h"
#include "nrf_userconfig.h"
#include "uart.h"
#include "nmea_gps.h"
#include "aes.h"
#include "utils.h"

#define UART_BAUD           9600
#define LED_R BIT0

struct Message
{
    float icTemperature;  // internal msp430 thermistor
    struct GPS gps;  // external uart gps module
};

char gpsBuffer[NMEA_SENT_LEN];
unsigned int gpsBufferIndex = 0;

struct Frame frame;
struct Message msg;

char *msgBuffer;

void nrfInit(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    P1DIR |= LED_R;
    P1OUT &= ~LED_R;

    struct AES_ctx ctx;
    uint8_t key[] = KEY;
    uint8_t iv[] = IV;

    serialBegin(UART_BAUD);
    nrfInit();
    msprf24_standby();

    while (1)
    {
        msg.icTemperature = 2.9f;
        msgBuffer = (char*) &msg;

        unsigned char fi;  // frame index
        for (fi = 0; fi < FRAME_NUMBER; fi++)
        {
            frame.index = fi;
            ch_arr_cpy(frame.data, msgBuffer, fi * FRAME_DATA_LEN,
                       (fi + 1) * FRAME_DATA_LEN);

            AES_init_ctx_iv(&ctx, key, iv);
            AES_CBC_encrypt_buffer(&ctx, (uint8_t*) frame.data, FRAME_DATA_LEN);

            w_tx_payload(BUF_SIZE, (uint8_t*) &frame);  //(uint8_t*) &frame);
            msprf24_activate_tx();

            /* Stop the main routine and listen for interrupts
             * and collect the sensor data.
             */
            LPM4;  // wait for ACK

            // Indicate if transmission possible
            if (rf_irq & RF24_IRQ_FLAGGED)
            {
                rf_irq &= ~RF24_IRQ_FLAGGED;
                msprf24_get_irq_reason();

                if (rf_irq & RF24_IRQ_TX)
                {
                    P1OUT &= ~LED_R;
                    flush_tx();
                }
                if (rf_irq & RF24_IRQ_TXFAILED)
                {
                    P1OUT |= LED_R;

                    /* Retransmit last payload in the case of transmission
                     * fail to be sure the message is received by PRX.
                     */
                    tx_reuse_lastpayload();
                    pulse_ce();
                }

                msprf24_irq_clear(rf_irq);
            }

            _delay_cycles(80000);  // Should be changed to IRQ control
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
    rf_channel = (uint8_t) NRF_CHANNEL;;

    msprf24_init();
    msprf24_set_pipe_packetsize(0, BUF_SIZE);
    msprf24_open_pipe(0, 1);

    // Address setup
    uint8_t addr[5] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00 };
    w_tx_addr(addr);
    w_rx_addr(0, addr);
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

            /* Trigger to exit low power mode if it is to
             * prevent deadlock caused cascaded inside LPM
             * microcontrollers.
             */
            LPM4_EXIT;
        }
    }

    // To keep secure the buffer
    if (gpsBufferIndex >= NMEA_SENT_LEN || UCA0RXBUF == '$')
    {
        gpsBufferIndex = 0;
    }

    gpsBuffer[gpsBufferIndex++] = UCA0RXBUF;
}
