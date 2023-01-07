/*
 * utils.h
 *
 *  Created on: Jan 7, 2023
 *      Author: Ahmet Yusuf Sirin
 */

#ifndef UTILS_H__
#define UTILS_H_

#define BUF_SIZE            32
#define FRAME_DATA_LEN      16
#define FRAME_NUMBER        3  // 1 + (sizeof(struct Message) - 1) / FRAME_DATA_LEN

#define KEY "PraiseBeToAllah"
#define IV  "TheMostMerciful"

struct Frame
{
    unsigned char index;
    char data[FRAME_DATA_LEN];
};

// Array manipulation functions
void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex);

#endif /* UTILS_H_ */
