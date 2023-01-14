#include <msp430.h>
#include <stdint.h>

#include "msprf24.h"
#include "nrf_userconfig.h"
#include "aes.h"
#include "utils.h"

#define LED_R BIT0

struct Frame *frame;
uint8_t buf[BUF_SIZE];

void nrfInit(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    P1DIR |= LED_R;
    P1OUT &= ~LED_R;

    struct AES_ctx ctx;
    uint8_t key[] = KEY;
    uint8_t iv[] = IV;

    nrfInit();

    if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state()))
    {
        flush_rx();
    }
    msprf24_activate_rx();
//    LPM4;

    while (1)
    {
        if (rf_irq & RF24_IRQ_FLAGGED)
        {
            msprf24_get_irq_reason();
        }

        if (rf_irq & RF24_IRQ_RX)
        {
            r_rx_payload(BUF_SIZE, buf);
            msprf24_irq_clear(RF24_IRQ_RX);

            frame = (struct Frame*) buf;

            AES_init_ctx_iv(&ctx, key, iv);
            AES_CBC_decrypt_buffer(&ctx, (uint8_t*) frame->data,
                                   FRAME_DATA_LEN);

            /* Redirect incoming frame to Module 3 without
             * waiting for ACK because no need. There is
             * no public key sharing for encryption.
             */
            w_tx_payload_noack(BUF_SIZE, (uint8_t*) frame);
//            w_tx_payload(BUF_SIZE, (uint8_t*) &frame);
            msprf24_activate_tx();
            msprf24_activate_rx();

            P1OUT ^= LED_R;
        }

//        LPM4;
    }
}

void nrfInit(void)
{
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    rf_crc = RF24_EN_CRC | RF24_CRCO;
    rf_addr_width = 5;
    rf_speed_power = RF24_SPEED_1MBPS | RF24_POWER_0DBM;
    rf_channel = (uint8_t) NRF_CHANNEL;

    msprf24_init();
    msprf24_set_pipe_packetsize(0, BUF_SIZE);
    msprf24_open_pipe(0, 1);

    // Address setup
    uint8_t addrRX[5] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00 };
    uint8_t addrTX[5] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
    w_tx_addr(addrTX);
    w_rx_addr(0, addrRX);
}
