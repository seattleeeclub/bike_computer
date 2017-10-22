/*
DisplayTask Definition
*/

///////////////////////////////////////////////////////////////////////
#include "DisplayTask.h"
#include "gpio_driver.h"
#include "lcd_driver.h"
#include "gpsGP20U7.h"

//////////////////////////////////////
//Variables
DisplayItem_t mDisplayItem = DISPLAY_ITEM_NONE;
DataLogState_t mDataLogState = DATA_LOG_OFF;
uint8_t mDataLogFileIndex = 0x00;           //appended to log file name string
uint8_t mDataLogFileName[FILE_NAME_BUFFER_LENGTH];
uint32_t counter = 0x00;

////////////////////////////////////////////////////
//DisplayTask_Init
//
void DisplayTask_Init()
{
    //init variables
    mDisplayItem = DISPLAY_ITEM_SPEED;

    //set the initial log file name
    memset(mDataLogFileName, 0x00, FILE_NAME_BUFFER_LENGTH);
    mDataLogFileIndex = 0x00;
    int n = sprintf(mDataLogFileName, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);
    
    //FreeRTOS Tasks
    xTaskCreate(DisplayTask, (const portCHAR *)"display", 4*configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

void DisplayTask_SetDisplayItem(DisplayItem_t item)
{
    mDisplayItem = item;
}

DisplayItem_t DisplayTask_GetDisplayItem(void)
{
    return mDisplayItem;
}

void DisplayTask_DisplayItemIncrease(void)
{
    if (mDisplayItem < DISPLAY_ITEM_POSITION)
        mDisplayItem++;
    else
        mDisplayItem = DISPLAY_ITEM_NONE;
}

void DisplayTask_DisplayItemDecrease(void)
{
    if (mDisplayItem > DISPLAY_ITEM_NONE)
        mDisplayItem--;
    else
        mDisplayItem = DISPLAY_ITEM_POSITION;
}



////////////////////////////////////////////
//Set the variable but also make a new file?
void DisplayTask_EnableDataLog(void)
{
    mDataLogState = DATA_LOG_ON;
}

void DisplayTask_DisableDataLog(void)
{
    mDataLogState = DATA_LOG_OFF;
}

void DisplayTask_SetDataLog(DataLogState_t state)
{
    if ((state == DATA_LOG_OFF) || (state == DATA_LOG_ON))
    {
        mDataLogState = state;
    }
}

DataLogState_t DisplayTask_GetDataLogState(void)
{
    return mDataLogState;
}


uint8_t DisplayTask_DataLogIndexGetCurrent(uint8_t* buffer)
{
    //print the log file string and index into file name
    memset(mDataLogFileName, 0x00, FILE_NAME_BUFFER_LENGTH);
    memset(buffer, 0x00, FILE_NAME_BUFFER_LENGTH);

    int n = sprintf(mDataLogFileName, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);
    n = sprintf(buffer, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);

    return n;
}

//increase/decrease the data log file, return the index, and load a buffer with file name
//assumes buffer is at least as big as mDataLogFileName
uint8_t DisplayTask_DataLogIndexIncrease(uint8_t* buffer)
{
    if (mDataLogFileIndex < FILE_NAME_INDEX_MAX)
        mDataLogFileIndex++;
    else
        mDataLogFileIndex = FILE_NAME_INDEX_MIN;    //roll back the prices

    //print the log file string and index into file name
    memset(mDataLogFileName, 0x00, FILE_NAME_BUFFER_LENGTH);
    memset(buffer, 0x00, FILE_NAME_BUFFER_LENGTH);

    int n = sprintf(mDataLogFileName, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);
    n = sprintf(buffer, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);

    return n;
}

//assumes buffer is at least as big as mDataLogFileName
uint8_t DisplayTask_DataLogIndexDecrease(uint8_t* buffer)
{
    if (mDataLogFileIndex > FILE_NAME_INDEX_MIN)
        mDataLogFileIndex--;
    else
        mDataLogFileIndex = FILE_NAME_INDEX_MAX;    //roll back the prices

    //print the log file string and index into file name
    memset(mDataLogFileName, 0x00, FILE_NAME_BUFFER_LENGTH);
    memset(buffer, 0x00, FILE_NAME_BUFFER_LENGTH);

    int n = sprintf(mDataLogFileName, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);
    n = sprintf(buffer, "%s%d.TXT", LOG_FILE_STRING, mDataLogFileIndex);

    return n;
}



//////////////////////////////////////////////////////
//DisplayTask
//
void DisplayTask(void *pvParameters)
{
    uint8_t buf[128];
    GPSData data;

    for(;;)
    {
        //look at the gps data
        GPS_FIFO_PeekData(&data, GPS_POSITION_FIFO_SIZE - 1);

        //Log data? 
        if (mDataLogState == DATA_LOG_ON)
        {
            //write the file name to the top of the lcd
            int n = sprintf(buf, "LOG: %s  ", mDataLogFileName);
            LCD_DrawString88KernLength(0, 1, buf, n);

            //finally, append the data to a file
            n = sprintf(buf, "%4d  MPH: %d.%d  Lat: %d.%d.%d   Lng: %d.%d.%d   \r\n", counter+=2,                 
            data.speedMph, data.speedMphFraction, 
            data.latDeg, data.latMin, data.latMinFraction,
            data.lngDeg, data.lngMin, data.lngMinFraction);

            //append the data to the current log file

            taskENTER_CRITICAL();
            SD_AppendData(mDataLogFileName, buf, n);
            taskEXIT_CRITICAL();

        }

        //display the appropriate item
        switch(mDisplayItem)
        {
            //menu is likely enabled in this state
            //don't over write it
            case DISPLAY_ITEM_NONE:
            {
                if (mDataLogState == DATA_LOG_ON)
                    LED_DisplayRed();
                else
                    LED_DisplayBlue();

                vTaskDelay(1000);
                LED_DisplayNone();
                vTaskDelay(1000);
                break;
            }

            //menu off, show speed only
            case DISPLAY_ITEM_SPEED:
            {
                if (mDataLogState == DATA_LOG_ON)
                    LED_DisplayRed();
                else
                    LED_DisplayGreen();

                int n = sprintf(buf, " MPH: %d.%d    ", data.speedMph, data.speedMphFraction);
                LCD_DrawString1624KernLength(2, 1, buf, n);

                vTaskDelay(1000);
                LED_DisplayNone();
                vTaskDelay(1000);
                                
                break;
            }

            //menu off, display a bunch of stuff
            case DISPLAY_ITEM_POSITION:
            {
                if (mDataLogState == DATA_LOG_ON)
                    LED_DisplayRed();
                else
                    LED_DisplayGreen();


                int n = sprintf(buf, "Lat: %d.%d.%d", data.latDeg, data.latMin, data.latMinFraction);
                LCD_DrawString88KernLength(2, 1, buf, n);

                n = sprintf(buf, "Lng: %d.%d.%d", data.lngDeg, data.lngMin, data.lngMinFraction);
                LCD_DrawString88KernLength(3, 1, buf, n);

                n = sprintf(buf, " MPH: %d.%d    ", data.speedMph, data.speedMphFraction);
                LCD_DrawString1624KernLength(2, 1, buf, n);

                vTaskDelay(1000);
                LED_DisplayNone();
                vTaskDelay(1000);

                break;
            }
            default:
            {   
                vTaskDelay(2000);
                break;
            }
        }
    }

    //clean exit - delete this task if ever makes it here
    vTaskDelete(NULL);
}

