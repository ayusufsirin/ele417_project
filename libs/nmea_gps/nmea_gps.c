/*
 * nmea_gps.c
 *
 *  Created on: Dec 26, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#include "nmea_gps.h"

#define FIND_AND_NUL(s, p, c) ( \
   (p) = strchr(s, c), \
   *(p) = '\0', \
   ++(p), \
   (p))

struct GPS parse(char *nmeaSentence) {
    struct GPS gps;

    gps.message_id = nmeaSentence;

    gps.time = FIND_AND_NUL(gps.message_id, gps.time, ',');
    gps.data_valid = FIND_AND_NUL(gps.time, gps.data_valid, ',');
    gps.raw_latitude = FIND_AND_NUL(gps.data_valid, gps.raw_latitude, ',');


    gps.latitude = atof(gps.raw_latitude);

    return;
}



