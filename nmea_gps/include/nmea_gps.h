/*
 * nmea_gps.h
 *
 *  Created on: Dec 26, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#ifndef NMEA_GPS_H_
#define NMEA_GPS_H_

#define NMEA_SENT_LEN       82  // nmea sentence max length

struct GPS
{
    float time;
    float latitude;
    char n_s;
    float longitude;
    char e_w;
    float speed;
    float cog;
    unsigned long date;
    char positioning_mode[4];
};

extern void parse(struct GPS *gps, char *nmeaSentence);

#endif /* NMEA_GPS_H_ */
