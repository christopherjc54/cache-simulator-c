#include "simulations.h"

//functions

//prints file contents
//WARNING: destructive to file contents!
void printFileContents_dummySimulation(Queue* fileContents) {
    char* line;

    while(!isEmpty(fileContents)) {
        line = (char*) dequeue(fileContents);
        printf("%s", line);
        free(line);
    }

    free(fileContents);
}

//prints trace data structure
//WARNING: destructive to file contents!
void printTraceData_dummySimulation(Queue* traceData) {
    traceItem* item;

    printf("PRINTING DATA STRUCT\n\n");
    while(!isEmpty(traceData)) {
        item = (traceItem*) dequeue(traceData);
        printf("lenOfInstr:  %-8x  dstM: %-8x\naddrOfInstr: %-8x  srcM: %-8x\n\n", item->lenOfInstr, item->dstM, item->addrOfInstr, item->srcM);
        //printf("lenOfInstr: %-8x\taddrOfInstr: %x\ndstM:       %-8x\tsrcM:        %-8x\n\n", item->lenOfInstr, item->addrOfInstr, item->dstM, item->srcM);
        free(item);
    }
    printf("PRINTING COMPLETE\n\n");

    free(traceData);
}