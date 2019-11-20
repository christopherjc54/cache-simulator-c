#include "simulation.h"

void accessCache(cacheStruct* cache, argStruct* args, varStruct* vars, int address, int bytesRead, resultDataStruct* resDt) {
    // [     TAG      |  INDEX  | OFFSET ]
    int tag = address >> (vars->index_size + vars->offset_size);
    int index = (address & (0xFFFFFFFF >> 32 - (vars->index_size + vars->offset_size))) >> vars->offset_size;
    int offset = address & (0xFFFFFFFF >> 32 - (vars->offset_size));
    // printf("Trace Data Address: 0x%X\n", address);
    // printf("Tag: 0x%X Index: 0x%X Offset: 0x%X\n\n", tag, index, offset);

    int i;
    int numBlocksAccessed = (int) ceil( (double) (offset + bytesRead)/args->block_size );

    int diff = numBlocksAccessed - (resDt->maxNumBlkAcsCnt - 1); //(account for zero-index)
    if(diff > 0) {
        numBlocksAccessed = (int*) realloc(numBlocksAccessed, sizeof(int)*(resDt->maxNumBlkAcsCnt + diff));
        for(i=0; i < diff; i++) resDt->numBlkAcsCnt[resDt->maxNumBlkAcsCnt + i] = 0;
        resDt->maxNumBlkAcsCnt += diff;
    }
    resDt->numBlkAcsCnt[numBlocksAccessed]++;

    for(i=0; i < numBlocksAccessed; i++) {
        rowStruct* row = getRowByIndex(cache, index);
        blockStruct* block = getBlockByTag(row, tag, args->associativity);

        //check for hit
        if(block != NULL && block->valid == true) { //hit
            resDt->cacheHits++;
            resDt->totalCycles += 1;
        } else { //miss
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
            if(block->valid == false) resDt->compulsoryMisses++;
            else resDt->conflictMisses++;
            block->tag = tag;
            block->valid = true;
            resDt->totalCycles += 3;
        }

        resDt->totalCacheAccesses++;
        index += 1;
    }
}

resultDataStruct* runSimulation(Queue* traceData, argStruct* args, varStruct* vars) {
    printf("STARTING SIMULATION\n\n");

    //initialize cache
    cacheStruct* cache = initializeCache(vars->total_indices*1024, args->associativity);
    printf("malloc'd %d rows in cache with %d blocks each (~%d KBs)\n\n", 
        vars->total_indices*1024, args->associativity, 
        (int) round( (double) (sizeof(cacheStruct) 
            + (sizeof(rowStruct)*vars->total_indices*1024) 
            + (sizeof(blockStruct)*args->associativity * (vars->total_indices*1024))) / 1024)); //actual cache implementation size in KB

    int i;
    traceItem* item;

    resultDataStruct* resDt = (resultDataStruct*) malloc(sizeof(resultDataStruct));
    resDt->cacheHits = 0, resDt->compulsoryMisses = 0, resDt->conflictMisses = 0;
    resDt->totalCycles = 0, resDt->totalInstructions = 0, resDt->totalCacheAccesses = 0;

    // for number of blocks accessed per instruction statistics
    resDt->numBlkAcsCnt = (int*) malloc(sizeof(int)*resDt->maxNumBlkAcsCnt); //number of block accesses per instruction count
                                                                      //(i.e. numBlkAcsCnt[1] = 5, 1 block access per instruction occured 5 times)
    resDt->maxNumBlkAcsCnt = 2 +1; //maximum number of block accesses for all instructions
                                   //+1 for zero-index 
    for(i=0; i < resDt->maxNumBlkAcsCnt; i++) resDt->numBlkAcsCnt[i] = 0;

    //process instructions
    while(!isEmpty(traceData)) {
        item = (traceItem*) dequeue(traceData);

        accessCache(cache, args, vars, item->addrOfInstr, item->lenOfInstr, resDt); // instruction access
        if(item->srcM != 0) accessCache(cache, args, vars, item->srcM, 4, resDt); // read data, assuming 4 bytes for all data accesses
        if(item->dstM != 0) accessCache(cache, args, vars, item->dstM, 4, resDt); // write data, assuming 4 bytes for all data accesses

        if(item->srcM != 0 || item->dstM != 0) resDt->totalCycles += 2;
        resDt->totalCycles += 2;
        resDt->totalInstructions++;
        free(item);
    }

    //show number of blocks accessed per instruction statistics
    for(i=1; i < resDt->maxNumBlkAcsCnt; i++) 
        printf("%d block%s accessed %d time%s in a single instruction\n", 
            i, (i != 1 ? "s" : ""), resDt->numBlkAcsCnt[i], (resDt->numBlkAcsCnt[i] != 1 ? "s" : ""));
    printf("\n");

    //save results
    vars->cache_hit_rate = (double) resDt->cacheHits/resDt->totalCacheAccesses * 100;
    vars->cpi = (double) resDt->totalCycles/resDt->totalInstructions;

    freeCache(cache);
    free(resDt->numBlkAcsCnt);
    printf("SIMULATION COMPLETE\n\n");
    return resDt;
}