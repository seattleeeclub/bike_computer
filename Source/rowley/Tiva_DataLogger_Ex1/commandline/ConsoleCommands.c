

/* System Dependencies */
#include <stdio.h>   

/* Project Dependencies */
#include "ConsoleCommands.h"
#include "CommandHandlers.h"

//command string, function, and helper string

CommandEntry COMMAND_TABLE[] =
{
    { "test", cmdTest, "Test Function"},
    { "task", cmdTask, "Task Stats: heap/list/stats"},
    { "button", cmdButton, "Button Stats (stats / clear)"},
    { "$GPRMC", cmdGPSData, "GPS Output - Reserved-Auto"},
    { "dir", cmdFileDirectory, "Print Directory"},
    { "build", cmdBuildFileDirectory, "Rebuild Directory"},

    { "file", cmdFileFunctions, "File Functions: new/delete/print/, file name"},
    { "append", cmdAppendData, "Append data to file (Args: filename, data...."},
    

    {NULL, NULL, NULL}                // End of table indicator. MUST BE LAST!!!
};

// COMMAND_TABLE is static and is defined above
const CommandEntry *CommandTable = COMMAND_TABLE;
