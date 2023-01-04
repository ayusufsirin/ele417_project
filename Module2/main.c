#include <msp430.h>
#include <nrf24l01.h>
#include <aes.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    char *payload;
    struct AES_ctx ctx;
    uint8_t key[] = "PraiseBeToAllah";
    uint8_t iv[] = "TheMostMerciful";

    AES_init_ctx_iv(&ctx, key, iv);

    nrfBeginRX();

    while (1)
    {
        payload = nrfReceive();
        if (nrfAvailable() > 0)
        {
            nrfBeginTX();
            AES_CBC_decrypt_buffer(&ctx, payload, sizeof(payload));
            nrfSend(payload);
            nrfBeginRX();
        }
    }
}
