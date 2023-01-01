/*
 * nmea_gps.c
 *
 *  Created on: Dec 26, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#include "include/nmea_gps.h"
#include <stdlib.h>
#include <string.h>

#define FIND_AND_NUL(s, p, c) ( \
   (p) = strchr(s, c), \
   *(p) = '\0', \
   ++(p), \
   (p))

void parse(struct GPS *gps, char *nmeaSentence)
{
    char *message_id = nmeaSentence;

//    if (strcmp(message_id, "$GPRMC") == 0) {
//        return;
//    }

    char *time = FIND_AND_NUL(message_id, time, ',');
    char *data_valid = FIND_AND_NUL(time, data_valid, ',');
    char *raw_latitude = FIND_AND_NUL(data_valid, raw_latitude, ',');
    char *n_s = FIND_AND_NUL(raw_latitude, n_s, ',');
    char *raw_longitude = FIND_AND_NUL(n_s, raw_longitude, ',');
    char *e_w = FIND_AND_NUL(raw_longitude, e_w, ',');
    char *speed = FIND_AND_NUL(e_w, speed, ',');
    char *cog = FIND_AND_NUL(speed, cog, ',');
    char *date = FIND_AND_NUL(cog, date, ',');
    char *magnetic_variation = FIND_AND_NUL(date, magnetic_variation, ',');
    char *m_e_w = FIND_AND_NUL(magnetic_variation, m_e_w, ',');
    char *positioning_mode = FIND_AND_NUL(m_e_w, positioning_mode, ',');

    gps->time = strtof(time, NULL);
    gps->latitude = strtof(raw_latitude, NULL);
    gps->n_s = n_s[0];
    gps->longitude = strtof(raw_longitude, NULL);
    gps->e_w = e_w[0];
    gps->speed = strtof(speed, NULL);
    gps->cog = strtof(cog, NULL);
    gps->date = strtol(date, NULL, 10);
    strcpy(gps->positioning_mode, positioning_mode);
}
