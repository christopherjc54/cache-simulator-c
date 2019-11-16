#ifndef _data_h
#define _data_h

#include <stdio.h> //for debug and error testing
#include "queue.h"
#include "main.h"

#define ssize_t signed long long //isn't defined in my compiler's libraries for some reason, but size_t is??

//function prototypes

Queue* readFile(char* fileName);
Queue* convertData(Queue* fileContents);

void printFileContents(Queue* fileContents);
void printTraceData(Queue* traceData);

#endif