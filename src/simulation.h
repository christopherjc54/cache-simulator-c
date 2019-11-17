#ifndef _simulation_h
#define _simulation_h

#include <stdio.h> //for debug and error testing
#include <string.h>
#include "queue.h"
#include "main.h"
#include "cache.h"

//function prototypes
void runSimulation(Queue* traceData, argStruct* args, varStruct* vars);

#endif