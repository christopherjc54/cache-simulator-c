#ifndef _main_h
#define _main_h

#pragma pack(1) //compiler directive to compact memory usage

//structs have to be before the includes to prevent some weird cross-referencing issue for the function prototype in simulations.h

typedef struct argStruct {
    char* trace_file_name;
    int cache_size; //in KB
    int block_size; //in bytes
    int associativity;
    char* replacement_policy; //will be RR, RND, or LRU
} argStruct;

typedef struct varStruct {
    int total_blocks; //in KB
    int index_size; //bits
    int offset_size; //bits
    int tag_size; //bits
    int total_indices; //in KB
    int overhead_memory_size; //in bytes
    int implementation_memory_size; //in bytes

    double cache_hit_rate; //percent
    double cpi; //cycles per instruction
} varStruct;

typedef struct traceItem {
    int lenOfInstr;
    int addrOfInstr;
    int dstM;
    int srcM;
} traceItem;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "data.h"
#include "simulation.h"
#include "queue.h"

#endif