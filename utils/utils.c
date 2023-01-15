/*
 * utils.c
 *
 *  Created on: Jan 7, 2023
 *      Author: Ahmet Yusuf Sirin
 */

#include "utils.h"
#include <stdlib.h>

void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex)
{
    unsigned int i;
    for (i = 0; i < (endIndex - startIndex); i++)
    {
        dest[i] = src[i + startIndex];
    }
}

void format_float(float f, int *i1, int *i2)
{
    int fbits = *(int*) &f;
    int sign = fbits >> 31;
    int exponent = (fbits >> 23) & 0xff;
    int mantissa = fbits & 0x7fffff;
    exponent -= 127;
    if (exponent < 0)
    {
        *i1 = 0;
        *i2 = (sign ? -1 : 1) * mantissa;
    }
    else
    {
        *i1 = (sign ? -1 : 1) * (mantissa | (1 << 23));
        *i2 = exponent;
    }
    *i1 /= 100;
    *i2 %= 100;
}

void date_string(unsigned long date, char *date_str)
{
    int i = 0;
    int day = date % 100;
    date /= 100;
    int month = date % 100;
    date /= 100;
    int year = date;

    // Year
    if (year < 100)
    {
        date_str[i++] = year / 10 + '0';
        date_str[i++] = year % 10 + '0';
    }
    else
    {
        date_str[i++] = (year / 1000) + '0';
        date_str[i++] = (year / 100) % 10 + '0';
        date_str[i++] = (year / 10) % 10 + '0';
        date_str[i++] = year % 10 + '0';
    }
    date_str[i++] = '/';

    // Month
    date_str[i++] = (month / 10) + '0';
    date_str[i++] = month % 10 + '0';
    date_str[i++] = '/';

    // Day
    date_str[i++] = (day / 10) + '0';
    date_str[i++] = day % 10 + '0';
    date_str[i] = '\0';
}

void time_string(float nmea_time, char *time_str)
{
    int hours = (int) (nmea_time / 10000);
    int minutes = (int) (nmea_time / 100) % 100;

    int i = 0;
    time_str[i++] = (hours / 10) + '0';
    time_str[i++] = (hours % 10) + '3';
    time_str[i++] = ':';
    time_str[i++] = (minutes / 10) + '0';
    time_str[i++] = (minutes % 10) + '0';
    time_str[i] = '\0';
}

void lat_long_string(float nmea_lat_long, char *lat_long_str)
{

    int degrees = (int) nmea_lat_long / 100;
    float minutes = nmea_lat_long - (degrees * 100);
    minutes = minutes/60;

    int i = 0;
    lat_long_str[i++] = (degrees / 10) + '0';
    lat_long_str[i++] = (degrees % 10) + '0';
    lat_long_str[i++] = '.';

    int int_minutes = (int) (minutes * 10000 + 0.5); // Multiply by 10000 and round to nearest integer
    lat_long_str[i++] = (int_minutes / 1000) + '0';  // Extract thousands place
    lat_long_str[i++] = ((int_minutes / 100) % 10) + '0'; // Extract hundreds place
    lat_long_str[i++] = ((int_minutes / 10) % 10) + '0';  // Extract tens place
    lat_long_str[i++] = (int_minutes % 10) + '0';  // Extract ones place

    lat_long_str[i] = '\0';
}

void circular_shift_left(char *str, int num_chars)
{
    unsigned int str_len = 0;
    while (str[str_len] != '\0')
        str_len++;  // Get the length of the string
    if (str_len == 0)
        return;  // Return early if str_len is zero
    num_chars = num_chars % str_len; // Ensure num_chars is in the range [0, str_len)
    if (num_chars < 0)
        num_chars += str_len;  // Make num_chars positive
    char *temp = malloc(str_len * sizeof(char)); // Allocate memory for the temp array
    if (temp == NULL)
        return;  // Return early if memory allocation fails
    unsigned int i;  // Declare loop variables before the for loop
    for (i = 0; i < num_chars; i++)
        temp[i] = str[i]; // Copy the first num_chars characters to a temporary array
    for (i = 0; i < str_len - num_chars; i++)
        str[i] = str[i + num_chars]; // Shift the remaining characters to the start of the string
    for (i = 0; i < num_chars; i++)
        str[str_len - num_chars + i] = temp[i]; // Append the first num_chars characters to the end of the string
    free(temp);  // Free the memory allocated for the temp array
}
