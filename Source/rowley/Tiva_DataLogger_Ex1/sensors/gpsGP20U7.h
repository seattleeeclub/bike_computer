//////////////////////////////////////////////
/*
GPS Module - GP-20U7
Serial GPS from Sparkfun

*/
//////////////////////////////////////////////




#ifndef __GPS_GP_20U7_H
#define __GPS_GP_20U7_H

#include <stddef.h>
#include <stdint.h>

#define GPS_POSITION_FIFO_SIZE      4
#define GPS_ITEM_BUFFER_SIZE        16
typedef struct
{
    uint8_t latRaw[GPS_ITEM_BUFFER_SIZE];       //raw gps data
    uint8_t lngRaw[GPS_ITEM_BUFFER_SIZE];       //raw gps data
    uint8_t speedRaw[GPS_ITEM_BUFFER_SIZE];     //raw gps data

    int latDeg;
    int latMin;
    int latMinFraction;         //4 decimal places

    int lngDeg;
    int lngMin;
    int lngMinFraction;         //4 decimal places

    int speedMph;               //mph
    int speedMphFraction;       //mph
    int speedFps;               //feet per second
    int speedFpsFraction;       //feet per second

}GPSData;



void GPS_init(void);
void GPS_FIFO_Clear(void);
void GPS_FIFO_Add(GPSData data);
void GPS_FIFO_PeekData(GPSData* data, uint16_t element);





#endif