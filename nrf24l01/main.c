/*
 * nrf24l01.c
 *
 *  Created on: Dec 28, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/nrf24l01.h"

#define RX_CONFIG_REG   0b00001111  // CRC '1' -> (2 bytes) & Power ON & Enable CRC & RECEIVER -------1
#define TX_CONFIG_REG   0b00001110

#define OUT_PIN_MASK    (MOSI + SCLK + CE + CSN)
#define IN_PIN_MASK     MISO

int i;
int j;
int k;
int l;  // for pload2, so many indexes..
int x;
int pd;  // payload function index 0-15
int pd_i;  // payload function i index 0-7
int pd_x;

int write_payload[TX_BUF_LEN / sizeof(int)];
unsigned char read_PAYLOAD[RX_BUF_LEN];  // RF FIFO
int pipe_nr;
int pyld1[8];  // 1st 16 bytes in RX FIFO
int pyld2[8];  // 2nd 16 bytes in RX FIFO

unsigned char status_reg;

char buf[5];
char pipe_nr_chr[5];

unsigned char clr_status[1] = { 0x70 };  // clear STATUS register

unsigned char rf_setupregister[1] = { 0b00000001 }; // Data Rate -> '0' (1 Mbps) & PA_MIN
unsigned char rf_chanregister[1] = { 0b01001100 };  // Channel '1001100'
unsigned char address[6] = "00001";  // write to RX_ADDR_P0 and TX_ADDR
unsigned char setup_retr_register[1] = { 0b01011111 };  // retry values
unsigned char en_aa_register[1] = { 0b00111111 };
unsigned char rx_pw_register[1] = { 0b00100000 }; // RX_payload width register -->32

void SCLK_Pulse(void);
void Send_Bit(unsigned int value);
void CE_On(void);  //Chip enable
void CE_Off(void);  //Chip disable
void CSN_On(void);     //CSN On
void CSN_Off(void);    //CSN Off
void Write_Byte(int content);
void Instruction_Byte_MSB_First(int content);
void Read_Byte_MSB_First(int index, unsigned char regname[]);
void Write_Byte_MSB_First(unsigned char content[], int index2);
void Write_Payload_MSB_First(int pyld[], int index3);
void init(unsigned char config_register[], int channel);
void ch_ptr_2_int_ptr(const char *charPtr, unsigned int size, int *intPtr);

void nrfBeginRX(int channel)
{
    unsigned char configregister[1] = { RX_CONFIG_REG };
    init(configregister, channel);
}

void nrfBeginTX(int channel)
{
    unsigned char configregister[1] = { TX_CONFIG_REG };
    init(configregister, channel);
}

void nrfSend(char *payload)
{
    ch_ptr_2_int_ptr(payload, TX_BUF_LEN, write_payload);

    //STDBY-I
    CSN_Off();
    Instruction_Byte_MSB_First(W_TX_PAYLOAD);
    Write_Payload_MSB_First(write_payload, 8);
    Write_Payload_MSB_First(write_payload, 8);
    CSN_On();

    CE_On();
    __delay_cycles(50); //min pulse >10usec
    CE_Off();

    //TX settling 130 usec
    __delay_cycles(150);
    //TX MODE

    __delay_cycles(20000);
    //STDBY-I

    CSN_Off();
    Instruction_Byte_MSB_First(NOP);
    CSN_On();

    if ((status_reg & BIT4) == 0x10)
    {
        CSN_Off();
        Instruction_Byte_MSB_First(W_REGISTER | STATUS);
        Write_Byte_MSB_First(clr_status, 1);
        CSN_On();

        CSN_Off();
        Instruction_Byte_MSB_First(FLUSH_TX);
        CSN_On();
    }
}

int nrfAvailable(void)
{
    CSN_Off();
    Instruction_Byte_MSB_First(NOP); //to get the status reg..
    CSN_On();

    if ((status_reg & BIT6) == 0x40)
        return 1;
    return 0;
}

unsigned char* nrfReceive(void)
{
    CE_On();
    __delay_cycles(150); //settling RX

    CSN_Off();
    Instruction_Byte_MSB_First(NOP); //to get the status reg..
    CSN_On();

    if ((status_reg & BIT6) == 0x40)
    {
        CE_Off();

        CSN_Off();
        Instruction_Byte_MSB_First(R_RX_PAYLOAD);
        Read_Byte_MSB_First(32, read_PAYLOAD);
        CSN_On();

        pipe_nr = status_reg & BIT4;
        ltoa(pipe_nr, pipe_nr_chr, 10);

        j = 0;
        l = 0;
        for (i = 0; i <= 14; i += 2)
        {
            pyld1[j] = read_PAYLOAD[i] | (read_PAYLOAD[i + 1] << 8);
            ltoa(pyld1[j], buf, 10);
            j++;

        }

        for (i = 16; i <= 30; i += 2)
        {
            pyld2[l] = read_PAYLOAD[i] | (read_PAYLOAD[i + 1] << 8);
            ltoa(pyld1[j], buf, 10);
            l++;
        }

        CSN_Off();
        Instruction_Byte_MSB_First(W_REGISTER | STATUS);
        Write_Byte_MSB_First(clr_status, 1);
        CSN_On();
    }
    return read_PAYLOAD;
}

void init(unsigned char config_register[], int channel)
{
    // Pin Setup
    P2DIR |= OUT_PIN_MASK;
    P2DIR &= ~IN_PIN_MASK;
    P2OUT &= ~OUT_PIN_MASK;

    __delay_cycles(100); //power on reset
    CE_Off();
    CSN_On();
    /************************
     **CONFIGURING REGISTERS**
     *************************/
    //EN_AA  -- enabling AA in all pipes
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | EN_AA);
    Write_Byte_MSB_First(en_aa_register, 1);
    CSN_On();
    //RF_SETUP
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RF_SETUP);
    Write_Byte_MSB_First(rf_setupregister, 1);
    CSN_On();
    //RX_ADDR_P0
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_ADDR_P0);
    Write_Byte_MSB_First(address, 5); // write 5 bytes address
    CSN_On();
    //TX_ADDR
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | TX_ADDR);
    Write_Byte_MSB_First(address, 5); // write 5 bytes address
    CSN_On();
    //RF_CH
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | channel);
    Write_Byte_MSB_First(rf_chanregister, 1);
    CSN_On();
    //SETUP_RETR
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | SETUP_RETR);
    Write_Byte_MSB_First(setup_retr_register, 1);
    CSN_On();
    //RX_PW0
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_PW_P0);
    Write_Byte_MSB_First(rx_pw_register, 1);
    CSN_On();
    //RX_PW1
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_PW_P1);
    Write_Byte_MSB_First(rx_pw_register, 1);
    CSN_On();
    //RX_PW2
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_PW_P2);
    Write_Byte_MSB_First(rx_pw_register, 1);
    CSN_On();
    //RX_PW3
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_PW_P3);
    Write_Byte_MSB_First(rx_pw_register, 1);
    CSN_On();
    //RX_PW4
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_PW_P4);
    Write_Byte_MSB_First(rx_pw_register, 1);
    CSN_On();
    //RX_PW4
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | RX_PW_P5);
    Write_Byte_MSB_First(rx_pw_register, 1);
    CSN_On();
    //CONFIG  (TX or RX)
    CSN_Off();
    Instruction_Byte_MSB_First(W_REGISTER | CONFIG);
    Write_Byte_MSB_First(config_register, 1);
    CSN_On();
    /****************************
     **END CONFIGURING REGISTERS**
     *****************************/
    __delay_cycles(2000);  //start_up >1.5 ms
}

void SCLK_Pulse(void)
{
    P2OUT |= SCLK;  // set high with OR 1
    P2OUT ^= SCLK;  // toggle with XOR 1
}

void Send_Bit(unsigned int value)
{
    if (value != 0)
        P2OUT |= MOSI;
    else
        P2OUT &= ~MOSI;
}

void CE_On(void)
{
    P2OUT |= CE;
}

void CE_Off(void)
{
    P2OUT &= ~CE;
}

void CSN_On(void)
{
    P2OUT |= CSN;
}

void CSN_Off(void)
{
    P2OUT &= ~CSN;
}

void Write_Byte(int content)
{
    for (j = 0; j < 8; j++)
    {
        x = (content & (1 << j));  // Write to Address
        Send_Bit(x);
        SCLK_Pulse();
    }
}

void Instruction_Byte_MSB_First(int content)
{
    for (k = 7; k >= 0; --k)
    {
        x = (content & (1 << k));  // Write to Address
        status_reg <<= 1;
        Send_Bit(x);

        if ((P2IN & MISO) == 0x02)
            status_reg |= 0b00000001;
        else
            status_reg &= 0b11111110;

        SCLK_Pulse();
    }
}

void Read_Byte_MSB_First(int index, unsigned char regname[])
{
    for (i = 0; i <= (index - 1); i++)
    {
        for (k = 0; k < 8; k++)
        {
            regname[i] <<= 1;

            if ((P2IN & MISO) == 0x02)
            {
                regname[i] |= 0b00000001;
            }
            else
            {
                regname[i] &= 0b11111110;
            }
            SCLK_Pulse();
        }
    }
}

void Write_Byte_MSB_First(unsigned char content[], int index2)
{
    for (i = 0; i <= (index2 - 1); i++)
    {
        for (k = 7; k >= 0; --k)
        {
            x = (content[i] & (1 << k));  // Write to Address
            Send_Bit(x);
            SCLK_Pulse();
        }
    }
}

void Write_Payload_MSB_First(int pyld[], int index3)
{
    for (pd_i = 0; pd_i <= (index3 - 1); pd_i++)
    {
        for (pd = 7; pd >= 0; --pd)
        {
            pd_x = (pyld[pd_i] & (1 << pd));  // Write to Address
            Send_Bit(pd_x);
            SCLK_Pulse();
        }
        for (pd = 15; pd >= 8; --pd)
        {
            pd_x = (pyld[pd_i] & (1 << pd));  // Write to Address
            Send_Bit(pd_x);
            SCLK_Pulse();
        }
    }
}

/* #################
 * Utility Functions
 * #################
 */
void ch_ptr_2_int_ptr(const char *charPtr, unsigned int size, int *intPtr)
{
    size = (sizeof(char) * size) / sizeof(int);
    unsigned int i;
    for (i = 0; i < size; i++)
    {
        intPtr[i] = (charPtr[2 * i + 1] << 8) | charPtr[2 * i];
    }
}
