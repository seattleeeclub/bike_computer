#include <stdio.h>

#include "console.h"

//command table
#include "ConsoleCommands.h"
#include "CommandHandlers.h"

#include "uart_driver.h"

#include <stdarg.h>

//IO buffers and command buffers
char cmdBuffer[CMD_BUFFER_SIZE];
uint32_t cmdIndex = 0x00;
char outBuffer[PRINT_BUFFER_SIZE];


typedef enum
{
    PS_WHITESPACE,
    PS_TOKEN,
    PS_STRING,
    PS_ESCAPE
}State;


//function definitions
//init items used by the command parser
void InitConsole(void)
{
  memset(cmdBuffer, 0x00, CMD_BUFFER_SIZE);
  memset(outBuffer, 0x00, PRINT_BUFFER_SIZE);
  cmdIndex = 0x00;

}

//print something over the usart or usb
//
int ConsolePrint(PortType_t port, const char *format, ...)
{
    memset (outBuffer, 0, PRINT_BUFFER_SIZE);

    va_list argPtr;
    va_start(argPtr, format);
    vsprintf(outBuffer, format, argPtr);
    va_end(argPtr);
    uint32_t length = strlen (outBuffer);

    uart_printArray(port, (uint8_t*)outBuffer, length);
    
    return length;
}


void ParseArgs(int *pArgc, char **argv)
{
    int argc = 0;
    char c;
    State lastState = PS_WHITESPACE;
    State stackedState = PS_WHITESPACE;
    char *argstr = cmdBuffer;

    // Tokenize the argstr
    while ((c = *argstr) != 0)
    {
        State newState;

        if (lastState == PS_ESCAPE)
        {
            newState = stackedState;
        }
        else if (lastState == PS_STRING)
        {
            if (c == '"')
            {
                newState = PS_WHITESPACE;
                *argstr = 0;
            }
            else
            {
                newState = PS_STRING;
            }
        }
        else if ((c == ' ') || (c == '\t') || (c == ','))
        {
            // Whitespace character
            *argstr = 0;
            newState = PS_WHITESPACE;
        }
        else if (c == '"')
        {
            newState = PS_STRING;
            *argstr++ = 0;
            argv[argc++] = argstr;
        }
        else if (c == '\\')
        {
            stackedState = lastState;
            newState = PS_ESCAPE;
        }
        else
        {
            // Token
            if (lastState == PS_WHITESPACE)
            {
                argv[argc++] = argstr;
            }
            newState = PS_TOKEN;
        }
        lastState = newState;
        argstr++;
    }
    argv[argc] = NULL;
    if (pArgc != NULL)
    {
        *pArgc = argc;
    }
}

int ExecCmd(PortType_t port, int argc, char **argv)
{
    int cmdNum;
    char *cmdStr;

    if (argc <= 0)
    {
        return TRUE;
    }

    cmdStr = argv[0];

   
    // Handle reserved commands
    if ( (strcmp(cmdStr, "?")   == 0) ||
         (strcmp(cmdStr, "help") == 0) )
    {
        PrintHelp(port);
        return TRUE;
    }

    cmdNum = ParseCmd(cmdStr);

    if (cmdNum < 0)
    {
        ConsolePrint (port, "Command not found\r\n");
    }

    else
    {
		//call command handler, forward on the source
    	//of the message
		(*CommandTable[cmdNum].cmdfunc)(port, argc, argv);
		return TRUE;
    }

    return TRUE;
}

//returns the index of the command in the command table
int ParseCmd(char *cmdStr)
{
 int cmdNum = 0;

    while (CommandTable[cmdNum].cmdstr != NULL)
    {
        if (strcmp(CommandTable[cmdNum].cmdstr, cmdStr) == 0)
        {
            return cmdNum;
        }
        cmdNum++;
    }
    // end of table reached
    return(-1);
}

void PrintHelp(PortType_t port)
{
    ConsolePrint (port, "Available commands:\r\n<?> or <help> - print this menu\r\n");

    uint32_t i = 0;
    while (CommandTable[i].cmdstr != NULL)
    {
        ConsolePrint (port, "%-16s - %s\r\n", CommandTable[i].cmdstr, CommandTable[i].helpstr);
        i++;
    }
}


