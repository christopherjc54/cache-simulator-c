#include "simulation.h"

void runSimulation(Queue* traceData, argStruct* args, varStruct* vars) {
    printf("STARTING SIMULATION\n\n");

    //initialize cache
    cacheStruct* cache = initializeCache(vars->total_indices*1024, args->associativity);
    printf("malloc'd %d rows in cache with %d blocks each\n\n", vars->total_indices*1024, args->associativity);

    traceItem* item;
    int cacheHits = 0;
    int cacheMisses = 0;
    int totalCycles = 0;
    int totalInstructions = 0;
    int numberOfReads;

    //go through instructions
    while(!isEmpty(traceData)) {
        // printf("made it here\n");
        item = (traceItem*) dequeue(traceData);

        // [     TAG      |  INDEX  | OFFSET ]
        int tag = item->addrOfInstr >> (vars->index_size + vars->offset_size);
        int index = (item->addrOfInstr & (0xFFFFFFFF >> 32 - (vars->index_size + vars->offset_size))) >> vars->offset_size;
        int offset = item->addrOfInstr & (0xFFFFFFFF >> 32 - (vars->offset_size));
        // printf("Trace Data Address: 0x%X\n", item->addrOfInstr);
        // printf("Tag: 0x%X Index: 0x%X Offset: 0x%X\n\n", tag, index, offset);

        rowStruct* row = getRowByIndex(cache, index);
        blockStruct* block = getBlockByTag(row, tag, args->associativity);

        //check for hit
        if(block != NULL && block->valid == true) {
            //TODO: figure out what happens on a cache hit
            cacheHits++;
            totalCycles++;
        } else {
            //replace block
            cacheMisses++;
            //TODO: implement number of reads for cache miss
            numberOfReads = 0;
            //pick a block for replacement
            if(strcmp(args->replacement_policy, "RR") == 0) {
                if(row->lastUsedIndex == -1 || row->lastUsedIndex + 1 == args->associativity) {
                    block = row->firstBlock;
                    row->lastUsedIndex = 0;
                } else {
                    block = getBlockByIndex(row, row->lastUsedIndex)->nextBlock;
                    row->lastUsedIndex++;
                }
            } else if(strcmp(args->replacement_policy, "RND") == 0) {
                block = getBlockByIndex(row, rand() % args->associativity);
            } else if(strcmp(args->replacement_policy, "LRU") == 0) {
                block = getLRUBlock(row);
                addOneTimeToAll(row);
                block->timeSinceLastUse = 0;
            }
            block->tag = tag;
            block->valid = true;
            totalCycles += 3 * numberOfReads;
        }

        //TODO: figure out what this is for
        item->lenOfInstr;

        //TODO: figure out what this is for
        if(item->srcM != 0); // read  // assume 4 bytes for all data accesses
        if(item->dstM != 0); // write

        if(item->srcM != 0 || item->dstM != 0) totalCycles += 2;
        totalCycles += 2;
        totalInstructions++;
        free(item);
    }

    //save results
    printf("total cycles: %d cache hits: %d cache misses: %d\ntotal instructions: %d\n\n", totalCycles, cacheHits, cacheMisses, totalInstructions);
    vars->cache_hit_rate = (double) cacheHits/totalInstructions * 100;
    vars->cpi = (double) totalCycles/totalInstructions;

    freeCache(cache);
    printf("SIMULATION COMPLETE\n\n");
}