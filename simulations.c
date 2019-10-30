#include "simulations.h"

//functions

//prints file contents
void printFileContents_dummySimulation(Queue* fileContents) {
    if(fileContents == NULL) {
        printf("error: fileContents is NULL\n");
        exit(-1);
    }
    LLNode* current = fileContents->qFront;

    printf("PRINTING FILE CONTENTS\n\n");
    while(current != NULL) {
        printf("%s", (char*) current->qt);
        current = current->pNext;
    }
    printf("PRINTING COMPLETE\n\n");
}

//prints trace data structure
void printTraceData_dummySimulation(Queue* traceData) {
    if(traceData == NULL) {
        printf("error: traceData is NULL\n");
        exit(-1);
    }
    LLNode* current = traceData->qFront;
    traceItem* item;

    printf("PRINTING DATA STRUCT\n\n");
    while(current != NULL) {
        item = (traceItem*) current->qt;
        printf("lenOfInstr:  %-8x  dstM: %-8x\naddrOfInstr: %-8x  srcM: %-8x\n\n", item->lenOfInstr, item->dstM, item->addrOfInstr, item->srcM);
        //printf("lenOfInstr: %-8x\taddrOfInstr: %x\ndstM:       %-8x\tsrcM:        %-8x\n\n", item->lenOfInstr, item->addrOfInstr, item->dstM, item->srcM);
        current = current->pNext;
    }
    printf("PRINTING COMPLETE\n\n");
}