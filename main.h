#ifndef _main_h
#define _main_h

#pragma pack(1)             //compiler directive to compact memory usage

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simulations.h"
#include "queue.h"

#define ssize_t signed long long //isn't defined in my compiler's libraries for some reason, but size_t is??

typedef struct traceItem {
    int lenOfInstr;
    int addrOfInstr;
    int dstM;
    int srcM;
} traceItem;

#endif