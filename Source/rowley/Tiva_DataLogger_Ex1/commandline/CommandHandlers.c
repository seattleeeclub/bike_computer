
#include <string.h>

#include "CommandHandlers.h"
#include "ConsoleCommands.h"
#include "console.h"
#include "uart_driver.h"
#include "ButtonTask.h"

#include "gpsGP20U7.h"


void cmdTest      (PortType_t port, int argc, char** argv)
{
    ConsolePrint(port, "Test Message\r\n");
}


////////////////////////////////////////////////
//Print task heap / list / stats / states
void cmdTask		(PortType_t port, int argc, char** argv)
{
	//task
	if (argc == 1)
	{
		ConsolePrint(port, "Args: heap / list / stats\r\n");
	}
	else if (argc == 2)
	{    
		memset(outBuffer, 0x00, PRINT_BUFFER_SIZE);

		if (!strcmp(argv[1], "heap"))
		{
			uint32_t size = xPortGetFreeHeapSize();
			ConsolePrint(port, "Free Heap: %d\r\n", size);
		}
		else if (!strcmp(argv[1], "list"))
		{
            ConsolePrint(port, "Name\t\tState\tPrio\tStk\tNum\r\n");
            ConsolePrint(port, "---------------------------------------------\r\n");
			vTaskList(outBuffer);
			uint32_t index = 0x00;
			while((outBuffer[index] != 0x00) && (index < PRINT_BUFFER_SIZE))
				index++;

            uart_printArray(port, (uint8_t*)outBuffer, index);

            ConsolePrint(port, "\r\n---------------------------------------------\r\n\r\n");
		}

		else if (!strcmp(argv[1], "stats"))
		{
			vTaskGetRunTimeStats(outBuffer);
			uint32_t index = 0x00;
			while((outBuffer[index] != 0x00) && (index < PRINT_BUFFER_SIZE))
				index++;

            uart_printArray(port, (uint8_t*)outBuffer, index);
		}

		else
			ConsolePrint(port, "Invalid Arg (1): heap / list / stats\r\n");
	}

	else
	{
		ConsolePrint(port, "Invalid Args\r\n");
	}
}


void cmdButton      (PortType_t port, int argc, char** argv)
{

	if (argc == 1)
	{
		ConsolePrint(port, "Args: stats / clear\r\n");
	}

	else if (argc == 2)
	{
		if (!strcmp(argv[1], "stats"))
		{

            ButtonStats stats = ButtonTask_GetStats();
			ConsolePrint(port, "ButtonStats:\r\nUp: %d\r\nDown: %d\r\nLeft: %d\r\nRight: %d\r\nSelect: %d\r\n",
                stats.numClicks_up, stats.numClicks_down, stats.numClicks_left, stats.numClicks_right, stats.numClicks_select);
		}

		else if (!strcmp(argv[1], "clear"))
		{
            ButtonTask_ClearStats();
            ButtonStats stats = ButtonTask_GetStats();
			ConsolePrint(port, "ButtonStats:\r\nUp: %d\r\nDown: %d\r\nLeft: %d\r\nRight: %d\r\nSelect: %d\r\n",
                stats.numClicks_up, stats.numClicks_down, stats.numClicks_left, stats.numClicks_right, stats.numClicks_select);
		}
    }

    else
    {
        ConsolePrint(port, "Invalid Args\r\n");        
    }
}


/////////////////////////////////////////////////
//GPSData
//Called when the header command comes in
//"$GPRMC"
//Typical output:
//$GPRMC,040134.00,A,4759.25341,N,12210.86879,W,1.202,,220817,,,A*69
//Args:
//arg0 - command
//arg1 - UTM coordinate
//arg2 - A - valid - add to fifo if valid
//arg3 - 4759.25341 - deg, min, min fraction...   ddmm.mmmm - lat
//arg4 - N
//arg5 - 12210.86879 - deg, min, min fraction...  dddmm.mmmm - lng
//arg6 - W
//arg7 - speed - knots 
void cmdGPSData      (PortType_t port, int argc, char** argv)
{
    //data valid
    if (!strcmp(argv[2], "A"))
    {
        //copy strings from args 3, 5, 7 into lat, lng, speed
        GPSData data;
        
        //init
        memset(data.latRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
        memset(data.lngRaw, 0x00, GPS_ITEM_BUFFER_SIZE);
        memset(data.speedRaw, 0x00, GPS_ITEM_BUFFER_SIZE);

        strncpy(data.latRaw, argv[3], GPS_ITEM_BUFFER_SIZE);
        strncpy(data.lngRaw, argv[5], GPS_ITEM_BUFFER_SIZE);
        strncpy(data.speedRaw, argv[7], GPS_ITEM_BUFFER_SIZE);

        GPS_FIFO_Add(data);
    }

    //print out all arguments
//    ConsolePrint(PORT_UART_0, "\r\nGPS DATA:\r\n");
//    for (int i = 0 ; i < argc ; i++)
//    {
//        ConsolePrint(PORT_UART_0, "%s :", argv[i]);
//    }
//        ConsolePrint(PORT_UART_0, "\r\n");

}







/////////////////////////////////////////////
//Print the contents of the sd card.  rebuild the
//directory file and read it into SD_readBuffer
//
void cmdFileDirectory (PortType_t port, int argc, char** argv)
{
    if (argc == 1)
    {
       // int result = SD_BuildDirectory("DIR.TXT");
        
        //clear the read buffer
        memset(SD_readBuffer, 0x00, SD_READ_BUFFER_SIZE);

        int size = SD_PrintFileToBuffer("DIR.TXT", SD_readBuffer, SD_READ_BUFFER_SIZE);


        if (size > 0)
        {
            //print the contents
            ConsolePrint(port, "Directory\r\n____________________________\r\n");
            uart_printArray(port, SD_readBuffer, size);
            ConsolePrint(port, "\r\n____________________________\r\n");
        }
        else
        {
            ConsolePrint(port, "Error in printing the directory\r\n");
        }
    }

    else
    {
        ConsolePrint(port, "Invalid Args\r\n");
    }
}


////////////////////////////////////////////////////
//
void cmdBuildFileDirectory (PortType_t port, int argc, char** argv)
{
    if (argc == 1)
    {
       taskENTER_CRITICAL();
       int result = SD_BuildDirectory("DIR.TXT");
       taskEXIT_CRITICAL();

       ConsolePrint(port, "Rebuilding file directory\r\n");
    }
    else
    {
       ConsolePrint(port, "Invalid Args\r\n");
    }       
}


///////////////////////////////////////////
//File Functions - new , delete, print
void cmdFileFunctions (PortType_t port, int argc, char** argv)
{
    if (argc == 1)
    {
        ConsolePrint(port, "Args: (new/delete/print), filename\r\n");
    }

    else if (argc == 3)
    {
        //arg1 - command, arg2 - file name
        if (!strcmp(argv[1], "new"))
        {
            int result = SD_FileCreate(argv[2]);
            if (result >= 0)
            {
                ConsolePrint(port, "Create New File: %s and rebuilding the directory\r\n", argv[2]);
            }
            else
            {
                ConsolePrint(port, "Error creating file, error code: %d\r\n%s\r\n", (-1*result), SD_GetStringFromFatCode((-1*result)));
            }
        }

        else if (!strcmp(argv[1], "delete"))
        {
            int result = SD_FileDelete(argv[2]);
            if (result >= 0)
            {
                ConsolePrint(port, "Delete File: %s and rebuilding the directory\r\n", argv[2]);
            }
            else
            {
                ConsolePrint(port, "Error deleting the file, error code: %d\r\n%s\r\n", (-1*result), SD_GetStringFromFatCode((-1*result)));
            }
        }

        else if (!strcmp(argv[1], "print"))
        {
            //clear the read buffer
            memset(SD_readBuffer, 0x00, SD_READ_BUFFER_SIZE);

            int size = SD_PrintFileToBuffer(argv[2], SD_readBuffer, SD_READ_BUFFER_SIZE);

            if (size > 0)
            {
                //print the contents
                ConsolePrint(port, "Directory\r\n____________________________\r\n");
                uart_printArray(port, SD_readBuffer, size);
                ConsolePrint(port, "\r\n____________________________\r\n");
            }
            else
            {
                ConsolePrint(port, "Error in printing the directory\r\n");
            }
        }

        else
        {
            ConsolePrint(port, "Invalid Args: %s (new, delete, print)\r\n", argv[1]);
        }
    }

    else
    {
        ConsolePrint(port, "Invalid Args\r\n");
    }
}


//////////////////////////////////////////////////
void cmdAppendData (PortType_t port, int argc, char** argv)
{
    if (argc == 1)
    {
        ConsolePrint(port, "Args: filename, data....\r\n");
    }

    if (argc == 2)
    {
        ConsolePrint(port, "Filename: %s, missing data\r\n", argv[1]);
    }

    //at least 3 args - cmd, filename, data0, data1, data2... 
    else if (argc > 2)
    {
        //arg1 - filename, remaining args is the data, 
        //rebuild the data as a single string with length
        //and call sd_append function

        //use the write buffer - check to see if the size of the write buffer
        //is bigger than the number of data args
        memset(SD_writeBuffer, 0x00, SD_WRITE_BUFFER_SIZE);

        uint8_t numWords = argc - 2;        //cli, filename, ... data
        uint16_t totalLength = 0x00;

        //get the total length of the message
		for (int i = 0 ; i < numWords ; i++)
		{
			//space for next word + space + null termination + \r + \n
			if ((totalLength + strlen(argv[2+i])) < (SD_WRITE_BUFFER_SIZE - 5))
			{
				totalLength += sprintf((char*)(SD_writeBuffer + totalLength), argv[2 + i]);

				if (i != (numWords - 1))
					totalLength += sprintf((char*)(SD_writeBuffer + totalLength), " ");
			}
			else
			{
				//buffer full, add \r\n + 0x00 at SD_writeBuffer[totalLength]
				SD_writeBuffer[totalLength] = 0x00;

				ConsolePrint(port, "SD write buffer full: %s\r\n", SD_writeBuffer);
				break;
			}
		}

        //with all bytes written to the buffer that will fit,
        //add a new line and 0x00.  
        SD_writeBuffer[totalLength++] = '\r';
        SD_writeBuffer[totalLength++] = '\n';
        SD_writeBuffer[totalLength] = 0x00;

        //append the buffer to the file "filename"
        int result = SD_AppendData(argv[1], SD_writeBuffer, totalLength);

        if (result < 0)
        {
            ConsolePrint(port, "Error - FATCode: %d\r\nString: %s\r\n", (-1*result), SD_GetStringFromFatCode(-1*result));
        }
        else if (result != totalLength)
        {
            ConsolePrint(port, "Error - Size of the data does not match the number of bytes written\r\n");
        }
        else
        {
            ConsolePrint(port, "Append Data(file: %s): %s\r\n", argv[1], SD_writeBuffer);
        }
    }
}









