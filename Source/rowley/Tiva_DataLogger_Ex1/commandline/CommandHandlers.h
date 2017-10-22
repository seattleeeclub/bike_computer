//Console.h
//Files for parsing commands received by the commands task
#ifndef __commandhandlers_H
#define __commandhandlers_H

#include <stdint.h>
#include "console.h"        //PortType_t definition

void cmdTest		(PortType_t port, int argc, char** argv);
void cmdTask		(PortType_t port, int argc, char** argv);
void cmdButton      (PortType_t port, int argc, char** argv);
void cmdGPSData     (PortType_t port, int argc, char** argv);

void cmdFileDirectory (PortType_t port, int argc, char** argv);
void cmdBuildFileDirectory (PortType_t port, int argc, char** argv);
void cmdFileFunctions (PortType_t port, int argc, char** argv);
void cmdAppendData (PortType_t port, int argc, char** argv);


#endif