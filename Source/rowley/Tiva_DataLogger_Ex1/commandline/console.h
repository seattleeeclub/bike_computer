//Console.h
//Files for parsing commands received by the commands task
#ifndef __CONSOLE_H
#define __CONSOLE_H

//includes - need to include stdint or stddef, etc.  unknown uint32_t

#include <stdint.h>
#include <stddef.h>

//defines
#define TRUE				1
#define FALSE				0

#define CMD_BUFFER_SIZE    128
#define ARG_BUFFER_SIZE    128
#define PRINT_BUFFER_SIZE  1024

//typedef uart
typedef enum PortType
{ 
    PORT_UART_0,
    PORT_UART_1,
}PortType_t;



//externs

//IO buffers and command buffers
extern char cmdBuffer[CMD_BUFFER_SIZE];
extern uint32_t cmdIndex;
extern char outBuffer[PRINT_BUFFER_SIZE];



//function proto
void InitConsole(void);
int ConsolePrint(PortType_t port, const char *format, ...);


//command parse functions
void ParseArgs(int *pArgc, char **argv);
int ExecCmd(PortType_t port, int argc, char **argv);
int ParseCmd(char *cmdStr);
void PrintHelp(PortType_t port);



#endif 
