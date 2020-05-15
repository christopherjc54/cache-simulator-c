#ifndef _main_automated_h
#define _main_automated_h

#include "main_shared.h" //required to prevent redefinition of "argStruct" and "varStruct"
#include <pthread.h>

typedef struct threadDataBlock {
    argStruct* args;
    varStruct* vars;
    resultDataStruct* resDt;
} threadDataBlock;

//function prototypes

void handleIncorrectUsage(char* errorMessage);
int main(int argc, char* argv[]);
void* programThread(void* threadData);

#endif