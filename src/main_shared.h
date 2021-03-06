#ifndef _main_shared_h
#define _main_shared_h

#pragma pack(1) //compiler directive to compact memory usage

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "data.h"
#include "queue.h"
#include "simulation.h"

typedef struct argStruct {
    char* trace_file_name;
    int cache_size; //in KB
    int block_size; //in bytes
    int associativity;
    char* replacement_policy; //will be RR, RND, or LRU
} argStruct;

typedef struct varStruct {
    int total_blocks;
    int index_size; //bits
    int offset_size; //bits
    int tag_size; //bits
    int total_indices;
    int overhead_memory_size; //in bytes
    int implementation_memory_size; //in bytes

    double cache_hit_rate; //percent
    double cpi; //cycles per instruction
} varStruct;

#endif