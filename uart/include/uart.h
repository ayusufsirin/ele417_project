/*
 * uart.h
 *
 *  Created on: Dec 29, 2022
 *      Author: ayusu
 */

#ifndef UART_H_
#define UART_H_

#define TXD     BIT2
#define RXD     BIT1

extern void serialBegin(int baud);
extern void serialWrite(char byte);
extern int serialAvailable(void);

#endif /* UART_H_ */
