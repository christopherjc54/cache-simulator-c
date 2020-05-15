#ifndef _data_h
#define _data_h

#include <stdio.h>
#include "queue.h"
#include "main_shared.h"

#define ssize_t signed long long //isn't defined in my compiler's libraries for some reason, but size_t is??

typedef struct traceItem {
    int lenOfInstr;
    int addrOfInstr;
    int dstM;
    int srcM;
} traceItem;

//function prototypes

Queue* readFile(char* fileName);
Queue* convertData(Queue* fileContents);

void printFileContents(Queue* fileContents);
void printTraceData(Queue* traceData);

#endif