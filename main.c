#include "main.h"

void handleIncorrectUsage() {
    printf("\nInvalid number of arguments. Please launch using\nSim.exe -f trace1.txt -s 1024 -b 16 -a 2 -r RR\n\n-f <trace file name> [ name of text file with the trace ]\n-s <cache size in KB> [ 1 KB to 8 MB ]\n-b <block size> [ 4 bytes to 64 bytes ]\n-a <associativity> [ 1, 2, 4, 8, 16 ]\n-r <replacement policy> [ RR, RND, or LRU ]\n\n");
    exit(-1);
}

int main(int argc, char* argv[]) {
    //check for correct usage
    if(argc % 2 == 0 || argc < 3 || argc > 11) {
        handleIncorrectUsage();
    }

    argStruct* args = (argStruct*) malloc(sizeof(argStruct));

    //initialize argument variables
    args->trace_file_name = NULL;
    args->cache_size = NULL; //in KB
    args->block_size = NULL; //in bytes
    args->associativity = NULL;
    args->replacement_policy = NULL; //will be RR, RND, or LRU

    //read in arguments
    int i;
    for(i=1; i < argc; i+=2 ) {
        if(strcmp(argv[i], "-f") == 0) {
            args->trace_file_name = argv[i+1];
        } else if(strcmp(argv[i], "-s") == 0) {
            args->cache_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-b") == 0) {
            args->block_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-a") == 0) {
            args->associativity = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-r") == 0) {
            args->replacement_policy = argv[i+1];
        }
    }

    //set defaults if needed
    //TODO: comment out printf's and add (default) after variables in input report :(
    bool usedDefault = false;
    if(args->trace_file_name == NULL) handleIncorrectUsage();
    if(args->cache_size == NULL) {
        args->cache_size = 1024;
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Cache size not specified, using default of 1024KB.\n");
    }
    if(args->block_size == NULL) {
        args->block_size = 16;
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Block size not specified, using default of 16 bytes.\n");
    }
    if(args->associativity == NULL) {
        args->associativity = 2;
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Associativity not specified, using default of 2.\n");
    }
    if(args->replacement_policy == NULL) {
        args->replacement_policy = "RR";
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Replacement policy not specified, using default of RR (Round Robin).\n");
    }
    if(usedDefault) printf("\n");

    //print input report
    printf("Cache Simulator CS 3853 Spring 2019 - Group #%d\n\nCmd Line: ", 9);
    //TODO: find out if we print the exe argument too
    for(i=1; i<argc; i++) {
        printf("%s", argv[i]);
        if(i < argc-1) printf(" ");
    }
    printf("\nTrace File: %s\nCache Size: %d\nBlock Size: %d\nAssociativity: %d\nR-Policy: %s\n\n", args->trace_file_name, args->cache_size, args->block_size, args->associativity, args->replacement_policy);

    varStruct* vars = (varStruct*) malloc(sizeof(varStruct));

    //initialize variables for output report
    vars->total_blocks = ((args->cache_size*1024) / args->block_size) / 1024; //in KB
    vars->index_size = round ( log10( (args->cache_size*1024) / (args->associativity * args->block_size) )/log10(2) ); //bits
    vars->offset_size = round( log10(args->block_size)/log10(2) ); //bits
    vars->tag_size = 32 - vars->offset_size - vars->index_size; //bits
    vars->total_indices = pow(2, vars->index_size) / 1024; //in KB
    //TODO: verify calculations for overhead & implementation memory sizes
    vars->overhead_memory_size = ( (1+vars->tag_size)*args->associativity * vars->total_indices ) / 8; //in bytes
    vars->implementation_memory_size = (vars->total_blocks * args->block_size) + vars->overhead_memory_size; //in bytes

    vars->cache_hit_rate = -1; //percent
    vars->cpi = -1; //cycles per instruction
    
    //read in file
    Queue* fileContents = readFile(args->trace_file_name); //queue consists of (char*) void* to each line
    printFileContents(fileContents); //prints file contents

    //process file
    Queue* traceData = convertData(fileContents);
    printTraceData(traceData);

    //run simulation
    runSimulation(traceData, args, vars);

    //free trace data
    freeQueue(traceData);

    //print output report
    printf("Cache Size: %d KB\n", args->cache_size);
    printf("Block Size: %d bytes\n", args->block_size);
    printf("Associativity: %d\n", args->associativity);
    printf("Policy: %s\n\n", args->replacement_policy);

    printf("----- Calculated Value -----\n");
    printf("Total #Blocks: %d KB (2^%d)\n", vars->total_blocks, (int) round(log10(vars->total_blocks*1024)/log10(2)) );
    printf("Tag Size: %d bits\n", vars->tag_size);
    printf("Index Size: %d bits, Total Indices: %d KB\n", vars->index_size, vars->total_indices);
    printf("Overhead Memory Size: %d bytes (or %d KB), Implementation Memory Size: %d (or %d KB)\n\n", 
        vars->overhead_memory_size*1024, vars->overhead_memory_size, 
        vars->implementation_memory_size, vars->implementation_memory_size/1024);
    
    printf("----- Results -----\n");
    printf("Cache Hit Rate: %.1f %%\n", vars->cache_hit_rate);
    printf("CPI: %.1f cycles/instruction\n\n", vars->cpi);

    return 0;
}