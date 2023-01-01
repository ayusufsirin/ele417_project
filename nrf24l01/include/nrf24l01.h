/*
 * nrf24l01.h
 *
 *  Created on: Dec 28, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#ifndef NRF24L01_H_
#define NRF24L01_H_

/* #############################################
 * Instruction Codes from Product Specifications
 * #############################################
 *
 * Read RX-payload: 1 – 32 bytes.A read operation will always start at byte 0.
 * Payload will be deleted from FIFO after it is read. Used in RX mode.
 *
 * Write TX-payload: 1 – 32 bytes. A write operation will always start at byte 0.
 * Used in TX mode.
 *
 * Flush TX FIFO: Used in TX mode.
 *
 * Flush RX FIFO: Used in RX mode Should not be executed during transmission of
 * acknowledge, i.e. acknowledge package will not be completed.
 *
 * Reuse TX PL: Used for a PTX device Reuse last sent payload. Packets will be
 * repeatedly resent as long as CE is high.
 */
#define R_REGISTER       (0b00000000)  // Read Registers
#define W_REGISTER       (0b00100000)  // Write Registers
#define R_RX_PAYLOAD     (0b01100001)  // Read RX-payload
#define W_TX_PAYLOAD     (0b10100000)  // Write TX-payload
#define FLUSH_TX         (0b11100001)  // Flush TX FIFO
#define FLUSH_RX         (0b11100010)  // Flush RX FIFO
#define REUSE_TX_PL      (0b11100011)  // Reuse TX PL
#define NOP              (0b11111111)  // No operation

/* ##################
 * Register Addresses
 * ##################
 *
 * Enable ‘Auto Acknowledgment’: Function Disable this functionality to be
 * compatible with nRF2401
 *
 * Receive address data pipe 0: 5 Bytes maximum length LSByte is written first.
 * Write the number of bytes defined by SETUP_AW)
 *
 * Receive address data pipe 1: 5 Bytes maximum length LSByte is written first.
 * Write the number of bytes defined by SETUP_AW)
 *
 * Receive address data pipe 2: Only LSB. MSBytes will be equal to RX_ADDR_P1[39:8]
 *
 * Receive address data pipe 3: Only LSB. MSBytes will be equal to RX_ADDR_P1[39:8]
 *
 * Receive address data pipe 4: Only LSB. MSBytes will be equal to RX_ADDR_P1[39:8]
 *
 * Receive address data pipe 5: Only LSB. MSBytes will be equal to RX_ADDR_P1[39:8]
 *
 * Transmit address: Used for a PTX device only. (LSByte is written first)
 * Set RX_ADDR_P0 equal to this address to handle automatic acknowledge if this
 * is a PTX device with Enhanced ShockBurst™ enabled
 */
#define CONFIG           (0x00)  // Configuration Register
#define EN_AA            (0x01)  // Enable ‘Auto Acknowledgment’
#define EN_RXADDR        (0x02)  // Enabled RX Addresses
#define SETUP_AW         (0x03)  // Setup of Address Widths
#define SETUP_RETR       (0x04)  // Setup of Automatic Transmission
#define RF_CH            (0x03)  // RF Channel
#define RF_SETUP         (0x06)  // RF Setup Register (Bit 3 -> data rate 0:1 Mbps
#define STATUS           (0x07)  // Status Register
#define OBSERVE_TX       (0x08)  // Transmit Observe Register
#define CD               (0x09)  // Carrier Detect
#define RX_ADDR_P0       (0x0A)  // Receive address data pipe 0
#define RX_ADDR_P1       (0x0B)  // Receive address data pipe 1
#define RX_ADDR_P2       (0x0C)  // Receive address data pipe 2
#define RX_ADDR_P3       (0x0D)  // Receive address data pipe 3
#define RX_ADDR_P4       (0x0E)  // Receive address data pipe 4
#define RX_ADDR_P5       (0x0F)  // Receive address data pipe 5
#define TX_ADDR          (0x10)  // Transmit address
#define RX_PW_P0         (0x11)
#define RX_PW_P1         (0x12)
#define RX_PW_P2         (0x13)
#define RX_PW_P3         (0x14)
#define RX_PW_P4         (0x15)
#define RX_PW_P5         (0x16)
#define FIFO_STATUS      (0x17)  //FIFO Status register

/* ##########
 * GPIO Setup
 * ##########
 */
#define MOSI    BIT0  // P2.0
#define MISO    BIT1  // P2.1
#define SCLK    BIT4  // P2.4
#define CE      BIT5  // P2.5
#define CSN     BIT3  // P2.3

/* ####################
 * Function Definitions
 * ####################
 */
#define TX_BUF_LEN  16  // byte
#define RX_BUF_LEN  32  // byte

extern void nrfBeginRX(void);
extern void nrfBeginTX(void);
extern void nrfSend(char* payload);
extern unsigned char* nrfReceive(void);
extern int nrfAvailable(void);

#endif /* NRF24L01_H_ */
