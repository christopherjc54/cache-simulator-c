#include "main.h"

void handleIncorrectUsage(char* errorMessage) {

    printf("\n%s Please launch using:\n", errorMessage);
    printf("Sim.exe -f trace1.txt -s 1024 -b 16 -a 2 -r RR\n\n");

    printf("-f <trace file name> [ name of text file with the trace ]\n");
    printf("-s <cache size in KB> [ 1 KB to 8 MB ]\n");
    printf("-b <block size> [ 4 bytes to 64 bytes ]\n");
    printf("-a <associativity> [ 1, 2, 4, 8, 16 ]\n");
    printf("-r <replacement policy> [ RR, RND, or LRU ]\n\n");

    printf("You can use without -f to see the results of the cache calculations.\n\n");

}

int main(int argc, char* argv[]) {
    //declare nested functions
    auto void setDefaults();
    auto void doCacheMath();
    auto void showCalculatedValues();

    //check for correct usage
    int maxArgs = 5;
    if((argc % 2 == 0 || argc < 3 || argc > 1 + (maxArgs * 2)) && argc != 1) {
        handleIncorrectUsage("Invalid number of arguments.");
        exit(-1);
    }

    //initialize argument variables
    argStruct* args = (argStruct*) malloc(sizeof(argStruct));
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

    //set defaults if needed and check for out of bound inputs
    bool usedDefault = false;
    varStruct* vars; //declared here so it can be free'd after use
    if(args->trace_file_name == NULL) {
        handleIncorrectUsage("Trace file name not provided.");
        usedDefault = true; //removes extra newline before printing defaults
        setDefaults();
        doCacheMath();
        showCalculatedValues();
        free(args);
        free(vars);
        exit(-1);
    }
    void setDefaults() {
        if(args->cache_size == NULL) {
            args->cache_size = 1024;
            if(!usedDefault) printf("\n");
            usedDefault = true;
            printf("Cache size not specified, using default of 1024KB.\n");
        } else if(args->cache_size < 1 || args->cache_size > 8*1024) {
            handleIncorrectUsage("Cache size out of bounds.");
            exit(-1);
        }
        if(args->block_size == NULL) {
            args->block_size = 16;
            if(!usedDefault) printf("\n");
            usedDefault = true;
            printf("Block size not specified, using default of 16 bytes.\n");
        } else if(args->block_size < 4 || args->block_size > 64) {
            handleIncorrectUsage("Block size out of bounds.");
            exit(-1);
        }
        if(args->associativity == NULL) {
            args->associativity = 2;
            if(!usedDefault) printf("\n");
            usedDefault = true;
            printf("Associativity not specified, using default of 2.\n");
        } else if(args->associativity < 1 || args->associativity > 16 || numberIsPower(args->associativity, 2) == false) {
            handleIncorrectUsage("Associativity out of bounds.");
            exit(-1);
        }
        if(args->replacement_policy == NULL) {
            args->replacement_policy = "RR";
            if(!usedDefault) printf("\n");
            usedDefault = true;
            printf("Replacement policy not specified, using default of RR (Round Robin).\n");
        } else if(strcmp(args->replacement_policy, "RR") != 0 
               && strcmp(args->replacement_policy, "RND") != 0 
               && strcmp(args->replacement_policy, "LRU") != 0) {
            handleIncorrectUsage("Unknown replacement policy specified.");
            exit(-1);
        }
        if(usedDefault) printf("\n");
    } setDefaults();

    //print input report
    printf("Cache Simulator CS 3853 Spring 2019 - Group #%d\n\nCmd Line: ", 9);
    for(i=1; i<argc; i++) {
        printf("%s", argv[i]);
        if(i < argc-1) printf(" ");
    }
    printf("\nTrace File: %s\nCache Size: %d\nBlock Size: %d\nAssociativity: %d\nR-Policy: %s\n\n", 
        args->trace_file_name, args->cache_size, args->block_size, args->associativity, args->replacement_policy);

    void doCacheMath() {
        //initialize variables for output report
        vars = (varStruct*) malloc(sizeof(varStruct));
        vars->total_blocks = (args->cache_size*1024) / args->block_size;
        vars->index_size = round(log( (args->cache_size*1024) / (args->associativity * args->block_size) )/log(2)); //bits
        vars->offset_size = round(log(args->block_size)/log(2)); //bits
        vars->tag_size = 32 - vars->offset_size - vars->index_size; //bits
        vars->total_indices = pow(2, vars->index_size);
        vars->overhead_memory_size = ( (1+vars->tag_size)*args->associativity * vars->total_indices ) / 8; //in bytes
        vars->implementation_memory_size = (vars->total_blocks * args->block_size) + vars->overhead_memory_size; //in bytes

        vars->cache_hit_rate = -1; //percent
        vars->cpi = -1; //cycles per instruction
    } doCacheMath();
    
    //read in file
    Queue* fileContents = readFile(args->trace_file_name); //queue consists of (char*) void* to each line
    // printFileContents(fileContents);

    //process file
    Queue* traceData = convertData(fileContents);
    // printTraceData(traceData);

    //run simulation
    resultDataStruct* resDt = runSimulation(traceData, args, vars);

    //print output report
    printf("Cache Size: %d KB\n", args->cache_size);
    printf("Block Size: %d bytes\n", args->block_size);
    printf("Associativity: %d\n", args->associativity);
    printf("Policy: %s\n\n", args->replacement_policy);

    void showCalculatedValues() {
        printf("----- Calculated Value -----\n");
        printf("Total #Blocks: %d (2^%d)\n", vars->total_blocks, (int) round(log(vars->total_blocks)/log(2)) );
        printf("Tag Size: %d bits\n", vars->tag_size);
        printf("Index Size: %d bits, Total Indices: %d\n", vars->index_size, vars->total_indices);
        printf("Overhead Memory Size: %d bytes (or %d KB), Implementation Memory Size: %d (or %d KB)\n\n", 
            vars->overhead_memory_size, vars->overhead_memory_size/1024, 
            vars->implementation_memory_size, vars->implementation_memory_size/1024);
    } showCalculatedValues();
    
    printf("----- Simulation Statistics -----\n");
    printf("Total Cache Accesses:   %d\n", resDt->totalCacheAccesses);
    printf("Cache Hits:             %d\n", resDt->cacheHits);
    printf("Cache Misses:           %d\n", resDt->totalCacheAccesses - resDt->cacheHits);
    printf("--- Compulsory Misses:  %d\n", resDt->compulsoryMisses);
    printf("--- Conflict Misses:    %d\n\n", resDt->conflictMisses);

    printf("----- Results -----\n");
    printf("Cache Hit Rate: %.1f %%\n", vars->cache_hit_rate);
    printf("Cache Miss Rate: %.1f %%\n", 100 - vars->cache_hit_rate);
    printf("CPI: %.1f cycles/instruction\n\n", vars->cpi);

    //free dynamically allocated memory
    free(resDt->numBlkAcsCntArry);
    free(resDt);
    free(args);
    free(vars);

    printf("PROGRAM COMPLETE\n\n");
    return 0;
}

bool numberIsPower(int number, int power) {
    return lowerPrecision(round(log(number)/log(power)), 2) == lowerPrecision(log(number)/log(power), 2);
}

double lowerPrecision(double number, int precision) {
    int factor = pow(10, precision);
    return floor(number*factor)/factor;
}