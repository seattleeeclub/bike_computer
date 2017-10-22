///////////////////////////////////////////////
/*
GPS Module
GP-20U7 - A simple serial GPS
Continuous output at 1 second GPS data
*/
//////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gpsGP20U7.h"

GPSData mPositionFIFO[GPS_POSITION_FIFO_SIZE];

///////////////////////////////////////
//init current and last postions, clear the position
//fifo, etc.
void GPS_init()
{
    GPS_FIFO_Clear();
}

////////////////////////////////////////////////////////
void GPS_FIFO_Clear()
{
    for (int i = 0 ; i < GPS_POSITION_FIFO_SIZE ; i++)
    {
        memset(mPositionFIFO[i].latRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
        memset(mPositionFIFO[i].lngRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
        memset(mPositionFIFO[i].speedRaw, 0x00, GPS_ITEM_BUFFER_SIZE);

        mPositionFIFO[i].latDeg = 0x00;
        mPositionFIFO[i].latMin = 0x00;
        mPositionFIFO[i].latMinFraction = 0x00;

        mPositionFIFO[i].lngDeg = 0x00;
        mPositionFIFO[i].lngMin = 0x00;
        mPositionFIFO[i].lngMinFraction = 0x00;

        mPositionFIFO[i].speedMph = 0x00;
        mPositionFIFO[i].speedMphFraction = 0x00;
        mPositionFIFO[i].speedFps = 0x00;
        mPositionFIFO[i].speedFpsFraction = 0x00;

    }
}

////////////////////////////////////////////
//GPS_FIFO_Add
//Add GPS data point from raw data.  GPSData
//struct is assumed to contain valid strings, null
//terminated.  It also assumes that the decoded
//values for lat, lng, deg, min,... etc have not
//been decoded and are dummy values
//ie, the function could be the same as taking 3 
//pointers.
void GPS_FIFO_Add(GPSData data)
{
    //shift up by one element
    for (int i = 1 ; i < GPS_POSITION_FIFO_SIZE ; i++)
    {
        //clear the top element one position above
        memset(mPositionFIFO[i - 1].latRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
        memset(mPositionFIFO[i - 1].lngRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
        memset(mPositionFIFO[i - 1].speedRaw, 0x00, GPS_ITEM_BUFFER_SIZE);

        //copy the current position into one position above
        strncpy(mPositionFIFO[i-1].latRaw, mPositionFIFO[i].latRaw, GPS_ITEM_BUFFER_SIZE);
        strncpy(mPositionFIFO[i-1].lngRaw, mPositionFIFO[i].lngRaw, GPS_ITEM_BUFFER_SIZE);
        strncpy(mPositionFIFO[i-1].speedRaw, mPositionFIFO[i].speedRaw, GPS_ITEM_BUFFER_SIZE);

        //copy the remaining members of GPS data
        mPositionFIFO[i-1].latDeg = mPositionFIFO[i].latDeg;
        mPositionFIFO[i-1].latMin = mPositionFIFO[i].latMin;
        mPositionFIFO[i-1].latMinFraction = mPositionFIFO[i].latMinFraction;

        mPositionFIFO[i-1].lngDeg = mPositionFIFO[i].lngDeg;
        mPositionFIFO[i-1].lngMin = mPositionFIFO[i].lngMin;
        mPositionFIFO[i-1].lngMinFraction = mPositionFIFO[i].lngMinFraction;

        mPositionFIFO[i-1].speedMph = mPositionFIFO[i].speedMph;
        mPositionFIFO[i-1].speedMphFraction = mPositionFIFO[i].speedMphFraction;
        mPositionFIFO[i-1].speedFps = mPositionFIFO[i].speedFps;
        mPositionFIFO[i-1].speedFpsFraction = mPositionFIFO[i].speedFpsFraction;

    }

    //clear the bottom element
    memset(mPositionFIFO[GPS_POSITION_FIFO_SIZE - 1].latRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
    memset(mPositionFIFO[GPS_POSITION_FIFO_SIZE - 1].lngRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
    memset(mPositionFIFO[GPS_POSITION_FIFO_SIZE - 1].speedRaw, 0x00, GPS_ITEM_BUFFER_SIZE);

    //copy data into the bottom element
    strncpy(mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].latRaw, data.latRaw, GPS_ITEM_BUFFER_SIZE);
    strncpy(mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].lngRaw, data.lngRaw, GPS_ITEM_BUFFER_SIZE);
    strncpy(mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].speedRaw, data.speedRaw, GPS_ITEM_BUFFER_SIZE);

    //decode raw data into the remaining members
    //typical values
    //$GPRMC,040134.00,A,4759.25341,N,12210.86879,W,1.202,,220817,,,A*69

    //Assume that we are staying in location where deg for lat long will
    //always be same number of digits - 5 decimal places
    //lat: 4759.25341 - ddmm.mmmmm
    //lng: 12210.86879 - ddmm.mmmmm
    //speed 1.202

    uint8_t tLatDeg[3] = {0x00, 0x00, 0x00};
    uint8_t tLatMin[3] = {0x00, 0x00, 0x00};
    uint8_t tLatMinFraction[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t tLngDeg[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t tLngMin[3] = {0x00, 0x00, 0x00};
    uint8_t tLngMinFraction[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    //pointers for collecting lat long info from raw strings
    uint8_t *ptrLat = data.latRaw;
    uint8_t *ptrLng = data.lngRaw;

    //latitude ## deg
    strncpy(tLatDeg, ptrLat, 2);            //raw lat deg into temp buffer
    ptrLat += 2;
    strncpy(tLatMin, ptrLat, 2);            //raw lat deg into temp buffer
    ptrLat += 3;                            //skip the period
    strncpy(tLatMinFraction, ptrLat, 5);    //raw lat deg into temp buffer

    //repeat for longitude - ### deg

    //do a check here for number of digits before the .  ??
    strncpy(tLngDeg, ptrLng, 3);            //raw lng deg into temp buffer
    ptrLng += 3;
    strncpy(tLngMin, ptrLng, 2);            //raw lng deg into temp buffer
    ptrLng += 3;                            //skip the period
    strncpy(tLngMinFraction, ptrLng, 5);    //raw lat deg into temp buffer


    float mph100 = (float)atof(data.speedRaw) * 115.078;     //100x
    float fps100 = mph100 * 1.46667;                         //100x

    int mph = (int)mph100;
    int mphInt = mph / 100;
    int mphDec = mph % 100;

    int fps = (int)fps100;
    int fpsInt = fps / 100;
    int fpsDec = fps % 100;

    //add elements to the GPSData struct
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].latDeg = atoi(tLatDeg);
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].latMin = atoi(tLatMin);
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].latMinFraction = atoi(tLatMinFraction);

    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].lngDeg = atoi(tLngDeg);
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].lngMin = atoi(tLngMin);
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].lngMinFraction = atoi(tLngMinFraction);

    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].speedMph = mphInt;
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].speedMphFraction = mphDec;
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].speedFps = fpsInt;
    mPositionFIFO[GPS_POSITION_FIFO_SIZE-1].speedFpsFraction = fpsDec;

}




//////////////////////////////////////////
//Looks at one element from the FIFO
//Does not remove it.
void GPS_FIFO_PeekData(GPSData* data, uint16_t element)
{
    //load the data pointer with element data
    strncpy(data->latRaw, mPositionFIFO[element].latRaw, GPS_ITEM_BUFFER_SIZE);
    strncpy(data->lngRaw, mPositionFIFO[element].lngRaw, GPS_ITEM_BUFFER_SIZE);
    strncpy(data->speedRaw, mPositionFIFO[element].speedRaw, GPS_ITEM_BUFFER_SIZE);

    data->latDeg = mPositionFIFO[element].latDeg;
    data->latMin = mPositionFIFO[element].latMin;
    data->latMinFraction = mPositionFIFO[element].latMinFraction;

    data->lngDeg = mPositionFIFO[element].lngDeg;
    data->lngMin = mPositionFIFO[element].lngMin;
    data->lngMinFraction = mPositionFIFO[element].lngMinFraction;

    data->speedMph = mPositionFIFO[element].speedMph;
    data->speedMphFraction = mPositionFIFO[element].speedMphFraction;
    data->speedFps = mPositionFIFO[element].speedFps;
    data->speedFpsFraction = mPositionFIFO[element].speedFpsFraction;

}