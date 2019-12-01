#include "main_automated.h"

void handleIncorrectUsage(char* errorMessage) {

        printf("\n%s Please launch using:\n", errorMessage);
        printf("Sim_Automated.exe -o results.csv\n\n");

        printf("-o <csv file name> [ name of text file to output results in csv format ]\n");
        printf("-c <file path> [ current directory file path ]\n\n");

        printf("You can use without -c if not using in nested folders.\n\n");

}

int main(int argc, char* argv[]) {
    int i, j, k, l, m;

    //handle incorrect usage
    if(argc % 2 == 0 || argc < 3 || argc > 5) {
        handleIncorrectUsage("Invalid number of arguments.");
        exit(-1);
    }

    //initialize argument variables
    char* csvFileName = NULL;
    char* currentFilePath = NULL;

    //read in arguments
    for(i=1; i < argc; i+=2 ) {
        if(strcmp(argv[i], "-o") == 0) {
            csvFileName = argv[i+1];
        } else if(strcmp(argv[i], "-c") == 0) {
            currentFilePath = atoi(argv[i+1]);
        }
    }

    //check for missing arguments
    if(csvFileName == NULL) {
        handleIncorrectUsage("CSV file name not specified.");
        exit(-1);
    } else if(currentFilePath == NULL) {
        printf("\nWARNING: current path not specified, make sure you're running this in the same folder as the traces directory\n\n");
        currentFilePath = "";
    }

    //set automated parameters
    #ifdef _WIN32
    char* traceFileNames[] = {  "traces\\TinyTrace.trc",
                                "traces\\TestTrace.trc",
                                "traces\\Corruption1.trc",
                                "traces\\Trace1A.trc",
                                "traces\\Trace2A.trc",
                                "traces\\A-9_new 1.5.pdf.trc" };
    int numTraceFileNames = 6;
    #else
    char* traceFileNames[] = {  "traces/TinyTrace.trc",
                                "traces/TestTrace.trc",
                                "traces/Corruption1.trc",
                                "traces/Trace1A.trc",
                                "traces/Trace2A.trc",
                                "traces/A-9_new 1.5.pdf.trc" };
    int numTraceFileNames = 6;
    #endif

    int cacheSizes[] = { 8, 64, 256, 1024 }; //in KB
    int numCacheSizes = 4;

    int blockSizes[] = { 4, 16, 64 }; //in bytes
    int numBlockSizes = 3;
    
    char* replacementPolicies[] = { "RR", "RND" };
    int numReplacementPolicies = 2;

    //TODO: figure out, not specified in instructions
    int associativities[] = { 2 };
    int numAssociativities = 1;

    //start a CSV file
    FILE* file = fopen(csvFileName, "w+");
    //add header row
    fprintf(file, "Trace File Name, Cache Size, Block Size, Replacement Policy, Associativity"); //basic run info
    fprintf(file, ", Total Blocks, Tag Size (bits), Index Size (bits), Total Indices, Overhead Memory Size (KB), Implementation Memory Size (KB)"); //calculated values
    fprintf(file, ", Total Cache Accesses, Cache Hits, Cache Misses, Compulsory Misses, Conflict Misses"); //statistics
    fprintf(file, ", Hit Rate %%, Miss Rate %%, CPI"); //results
    fflush(file);

    //run all automated loops
    for(i=0; i < numTraceFileNames; i++) {
        for(j=0; j < numCacheSizes; j++) {
            for(k=0; k < numBlockSizes; k++) {
                for(l=0; l < numReplacementPolicies; l++) {
                    for(m=0; m < numAssociativities; m++) {

                        //initialize arguments
                        argStruct* args = (argStruct*) malloc(sizeof(argStruct));
                        char* tmpFileName = (char*) malloc(strlen(currentFilePath) + strlen(traceFileNames[i]) + 1);
                        tmpFileName[0] = '\0';
                        strcat(tmpFileName, currentFilePath);
                        strcat(tmpFileName, traceFileNames[i]);
                        args->trace_file_name = tmpFileName;
                        args->cache_size = cacheSizes[j];
                        args->block_size = blockSizes[k];
                        args->replacement_policy = replacementPolicies[l];
                        args->associativity = associativities[m];

                        //initialize variables for output report
                        varStruct* vars = (varStruct*) malloc(sizeof(varStruct));
                        vars->total_blocks = (args->cache_size*1024) / args->block_size;
                        vars->index_size = round(log( (args->cache_size*1024) / (args->associativity * args->block_size) )/log(2)); //bits
                        vars->offset_size = round(log(args->block_size)/log(2)); //bits
                        vars->tag_size = 32 - vars->offset_size - vars->index_size; //bits
                        vars->total_indices = (args->cache_size*1024) / (args->associativity * args->block_size);
                        vars->overhead_memory_size = ( (1+vars->tag_size)*args->associativity * vars->total_indices ) / 8; //in bytes
                        vars->implementation_memory_size = (vars->total_blocks * args->block_size) + vars->overhead_memory_size; //in bytes

                        vars->cache_hit_rate = -1; //percent
                        vars->cpi = -1; //cycles per instruction

                        //run program
                        resultDataStruct* resDt = runProgram(args, vars);

                        //save results by adding row to CSV
                        fprintf(file, "\n");
                        fprintf(file, "\"%s\", %d, %d, \"%s\", %d", args->trace_file_name, args->cache_size, args->block_size, args->replacement_policy, args->associativity);
                        fprintf(file, ", %d, %d, %d, %d, %d, %d", vars->total_blocks, vars->tag_size, vars->index_size, vars->total_indices, vars->overhead_memory_size, vars->implementation_memory_size);
                        fprintf(file, ", %d, %d, %d, %d, %d", resDt->totalCacheAccesses, resDt->cacheHits, resDt->totalCacheAccesses - resDt->cacheHits, resDt->compulsoryMisses, resDt->conflictMisses);
                        fprintf(file, ", %.1f, %.1f, %.1f", vars->cache_hit_rate, 100 - vars->cache_hit_rate, vars->cpi);
                        fflush(file);

                        //free dynamically allocated memory
                        free(resDt->numBlkAcsCntArry);
                        free(resDt);
                        free(args->trace_file_name);
                        free(args);
                        free(vars);

                    }
                }
            }
        }
    }

    fclose(file);
    return 0;
}

resultDataStruct* runProgram(argStruct* args, varStruct* vars) {

    //print output report
    printf("Trace File: %d\n", args->trace_file_name);
    printf("Cache Size: %d KB\n", args->cache_size);
    printf("Block Size: %d bytes\n", args->block_size);
    printf("Associativity: %d\n", args->associativity);
    printf("Policy: %s\n\n", args->replacement_policy);

    printf("----- Calculated Value -----\n");
    printf("Total #Blocks: %d (2^%d)\n", vars->total_blocks, (int) round(log(vars->total_blocks)/log(2)) );
    printf("Tag Size: %d bits\n", vars->tag_size);
    printf("Index Size: %d bits, Total Indices: %d\n", vars->index_size, vars->total_indices);
    printf("Overhead Memory Size: %d bytes (or %d KB), Implementation Memory Size: %d (or %d KB)\n\n", 
        vars->overhead_memory_size, vars->overhead_memory_size/1024, 
        vars->implementation_memory_size, vars->implementation_memory_size/1024);

    //read in file
    Queue* fileContents = readFile(args->trace_file_name); //queue consists of (char*) void* to each line

    //process file
    Queue* traceData = convertData(fileContents);

    //run simulation
    resultDataStruct* resDt = runSimulation(traceData, args, vars);
    
    printf("----- Simulation Statistics -----\n");
    printf("Total Cache Accesses:\t%d\n", resDt->totalCacheAccesses);
    printf("Cache Hits:\t\t%d\n", resDt->cacheHits);
    printf("Cache Misses:\t\t%d\n", resDt->totalCacheAccesses - resDt->cacheHits);
    printf("--- Compulsory Misses:\t%d\n", resDt->compulsoryMisses);
    printf("--- Conflict Misses:\t%d\n\n", resDt->conflictMisses);

    printf("----- Results -----\n");
    printf("Cache Hit Rate: %.1f %%\n", vars->cache_hit_rate);
    printf("Cache Miss Rate: %.1f %%\n", 100 - vars->cache_hit_rate);
    printf("CPI: %.1f cycles/instruction\n\n", vars->cpi);

    printf("=======================================================================================\n");
    printf("=======================================================================================\n\n");

    return resDt;
}