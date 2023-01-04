/*
 * gy61.h
 *
 *  Created on: Jan 1, 2023
 *      Author: Ahmet Yusuf Sirin
 */

#ifndef GY61_H_
#define GY61_H_

#define GY61_PIN_X BIT3  // P1.3
#define GY61_PIN_Y BIT4  // P1.4
#define GY61_PIN_Z BIT5  // P1.5

extern void gy61Read(int x, int y, int z);  // unit: 1000m/s^2

#endif /* GY61_H_ */
