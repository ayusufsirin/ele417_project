/*
 * utils.h
 *
 *  Created on: Jan 7, 2023
 *      Author: Ahmet Yusuf Sirin
 */

#ifndef UTILS_H__
#define UTILS_H_

#define BUF_SIZE            32
#define PACKET_PAYLOAD_LEN      16
#define PACKET_NUMBER        3  // 1 + (sizeof(struct Message) - 1) / FRAME_DATA_LEN

#define NRF_CHANNEL         99

#define KEY "PraiseBeToAllah"
#define IV  "TheMostMerciful"

#define GMT +3

struct Packet
{
    unsigned char index;
    char payload[PACKET_PAYLOAD_LEN];
};

extern void ch_arr_cpy(char *dest, const char *src, int startIndex, int endIndex);
extern void format_float(float f, int *i1, int *i2);
extern void date_string(unsigned long date, char *date_str);
extern void time_string(float nmea_time, char *time_str);
extern void lat_long_string(float nmea_lat_long, char *lat_long_str);
extern void circular_shift_left(char *str, int num_chars);

#endif /* UTILS_H_ */
