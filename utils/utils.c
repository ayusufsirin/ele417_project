/*
 * utils.c
 *
 *  Created on: Jan 7, 2023
 *      Author: Ahmet Yusuf Sirin
 */

#include "utils.h"

void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex)
{
    unsigned int i;
    for (i = 0; i < (endIndex - startIndex); i++)
    {
        dest[i] = src[i + startIndex];
    }
}
