////////////////////////////////////////////////
//Console commands -
//struct typedef for command entry and table def
//
#ifndef CONSOLE_COMMANDS_H
#define CONSOLE_COMMANDS_H

#include "console.h"            //PortType_t

// Command entry definition
typedef struct
{
    char const *cmdstr;                      //command string
    void (*cmdfunc)(PortType_t port, int argc, char **argv);  //function pointer - command handler
    char const *helpstr;                     //help string
}CommandEntry;
      
extern const CommandEntry *CommandTable;    //an array defined as "CommandTable"


#endif
