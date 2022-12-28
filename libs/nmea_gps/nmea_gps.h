/*
 * nmea_gps.h
 *
 *  Created on: Dec 26, 2022
 *      Author: Ahmet Yusuf Sirin
 */

#ifndef NMEA_GPS_NMEA_GPS_H_
#define NMEA_GPS_NMEA_GPS_H_

struct GPS
{
    char *message_id;
    char *time;
    char *data_valid;
    char *raw_latitude;
    char *n_s;
    char *raw_longitude;
    char *e_w;
    char *speed;
    char *cog;
    char *date;
    char *magnetic_variation;
    char *m_e_w;
    char *positioning_mode;

    double latitude;
    double longtitude;
};

extern struct GPS parse(char *nmeaSentence);


#endif /* NMEA_GPS_NMEA_GPS_H_ */
