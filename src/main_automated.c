#include "main_automated.h"

void handleIncorrectUsage(char* errorMessage) {

        printf("\n%s Please launch using:\n", errorMessage);
        printf("Sim_Automated.exe -o results.csv -c \"..\" -t 1\n\n");

        printf("-o <csv file name> [ name of text file to output results in csv format ]\n");
        printf("-c <file path> [ current directory file path ]\n");
        printf("-t <number of threads> [ number of threads, \"optimal\", or \"max\" ]\n\n");

        printf("You can use without -c if not using in nested folders.\n");
        printf("You can use without -t. Defaults to 1 thread which allows for proper logging.\n");
        printf("Highly recommend piping standard out to log file. If using more than 1 thread, log is only good for errors.\n\n");

}

int main(int argc, char* argv[]) {
    int i, j, k, l, m, n;

    //handle incorrect usage
    if(argc % 2 == 0 || argc < 3 || argc > 7) {
        handleIncorrectUsage("Invalid number of arguments.");
        exit(-1);
    }

    //initialize argument variables
    char* csvFileName = NULL;
    char* currentFilePath = NULL;
    int numThreads = -1;

    //read in arguments
    for(i=1; i < argc; i+=2 ) {
        if(strcmp(argv[i], "-o") == 0) {
            csvFileName = argv[i+1];
        } else if(strcmp(argv[i], "-c") == 0) {
            currentFilePath = argv[i+1];
        } else if(strcmp(argv[i], "-t") == 0) {
            if(strcmp(argv[i+1], "optimal") == 0) {
                numThreads = -2;
            } else if(strcmp(argv[i+1], "max") == 0) {
                numThreads = -3;
            } else {
                numThreads = atoi(argv[i+1]);
            }
        }
    }

    //check for missing arguments
    if(csvFileName == NULL) {
        handleIncorrectUsage("CSV file name not specified.");
        exit(-1);
    }
    if(currentFilePath == NULL) {
        printf("WARNING: current path not specified\n");
        printf("Make sure you're running this in the same folder as the traces directory or go to sub-folder and use -c \"..\" to go up one level and look for it\n\n");
        currentFilePath = "";
    }
    if(numThreads == -1) {
        printf("Number of threads not specified, using 1 (recommended for logging purposes).\n\n");
        numThreads = 1;
    }

    //set automated parameters
    char* traceFileNames[] = {  "/traces/TinyTrace.trc",
                                "/traces/TestTrace.trc",
                                "/traces/Corruption1.trc",
                                "/traces/Trace1A.trc",
                                "/traces/Trace2A.trc",
                                "/traces/A-9_new 1.5.pdf.trc" };
    int numTraceFileNames = 6;

    int cacheSizes[] = { 8, 64, 256, 1024 }; //in KB
    int numCacheSizes = 4;

    int blockSizes[] = { 4, 16, 64 }; //in bytes
    int numBlockSizes = 3;
    
    char* replacementPolicies[] = { "RR", "RND" };
    int numReplacementPolicies = 2;

    //TODO: figure out, not specified in instructions
    int associativities[] = { 2 };
    int numAssociativities = 1;

    //set optimal or max threads if requested
    if(numThreads == -2) { //optimal
        numThreads = numCacheSizes * numBlockSizes * numReplacementPolicies * numAssociativities;
    } else if(numThreads == -3) { //max
        numThreads = numTraceFileNames * numCacheSizes * numBlockSizes * numReplacementPolicies * numAssociativities;
    }

    //start a CSV file
    FILE* csvFile = fopen(csvFileName, "w+");
    if(csvFile == NULL) {
        printf("Error opening %s\n", csvFileName);
        exit(-1);
    }
    //add header row
    fprintf(csvFile, "Trace File Name, Cache Size, Block Size, Replacement Policy, Associativity"); //basic run info
    fprintf(csvFile, ", Total Blocks, Tag Size (bits), Index Size (bits), Total Indices, Overhead Memory Size (KB), Implementation Memory Size (KB)"); //calculated values
    fprintf(csvFile, ", Total Cache Accesses, Cache Hits, Cache Misses, Compulsory Misses, Conflict Misses"); //statistics
    fprintf(csvFile, ", Hit Rate %%, Miss Rate %%, CPI"); //results
    fflush(csvFile);

    int remainingThreads = numThreads;
    pthread_t tid[numThreads];

    //run all possible combinations
    for(i=0; i < numTraceFileNames; i++) {
        for(j=0; j < numCacheSizes; j++) {
            for(k=0; k < numBlockSizes; k++) {
                for(l=0; l < numReplacementPolicies; l++) {
                    for(m=0; m < numAssociativities; m++) {

                        threadDataBlock* thDt = (threadDataBlock*) malloc(sizeof(threadDataBlock));

                        //initialize arguments
                        thDt->args = (argStruct*) malloc(sizeof(argStruct));
                        char* tmpFileName = (char*) malloc(strlen(currentFilePath) + strlen(traceFileNames[i]) + 1);
                        tmpFileName[0] = '\0';
                        strcat(tmpFileName, currentFilePath);
                        strcat(tmpFileName, traceFileNames[i]);
                        thDt->args->trace_file_name = tmpFileName;
                        thDt->args->cache_size = cacheSizes[j];
                        thDt->args->block_size = blockSizes[k];
                        thDt->args->replacement_policy = replacementPolicies[l];
                        thDt->args->associativity = associativities[m];

                        //initialize variables for output report
                        thDt->vars = (varStruct*) malloc(sizeof(varStruct));
                        thDt->vars->total_blocks = (thDt->args->cache_size*1024) / thDt->args->block_size;
                        thDt->vars->index_size = round(log( (thDt->args->cache_size*1024) / (thDt->args->associativity * thDt->args->block_size) )/log(2)); //bits
                        thDt->vars->offset_size = round(log(thDt->args->block_size)/log(2)); //bits
                        thDt->vars->tag_size = 32 - thDt->vars->offset_size - thDt->vars->index_size; //bits
                        thDt->vars->total_indices = (thDt->args->cache_size*1024) / (thDt->args->associativity * thDt->args->block_size);
                        thDt->vars->overhead_memory_size = ( (1+thDt->vars->tag_size)*thDt->args->associativity * thDt->vars->total_indices ) / 8; //in bytes
                        thDt->vars->implementation_memory_size = (thDt->vars->total_blocks * thDt->args->block_size) + thDt->vars->overhead_memory_size; //in bytes

                        thDt->vars->cache_hit_rate = -1; //percent
                        thDt->vars->cpi = -1; //cycles per instruction

                        //run thread
                        pthread_create(&tid[numThreads - remainingThreads], NULL, programThread, (void*) thDt);
                        remainingThreads--;

                        //check on running threads
                        if(remainingThreads <= 1 || (i == numTraceFileNames - 1 && j == numCacheSizes - 1 && k == numBlockSizes - 1 && l == numReplacementPolicies - 1 && m == numAssociativities - 1)) {
                            //wait for threads
                            threadDataBlock* thDt_r[numThreads]; //for returned thread data
                            for(n=0; n < numThreads - remainingThreads; n++) {
                                pthread_join(tid[n], (void**) &thDt_r[n]);
                            }
                            //save data from threads
                            for(n=0; n < numThreads && n < numThreads - remainingThreads; n++) {
                                //save results by adding row to CSV
                                fprintf(csvFile, "\n");
                                fprintf(csvFile, "\"%s\", %d, %d, %s, %d", //basic run info
                                    thDt_r[n]->args->trace_file_name, 
                                    thDt_r[n]->args->cache_size, 
                                    thDt_r[n]->args->block_size, 
                                    thDt_r[n]->args->replacement_policy, 
                                    thDt_r[n]->args->associativity);
                                fprintf(csvFile, ", %d, %d, %d, %d, %d, %d", //calculated values
                                    thDt_r[n]->vars->total_blocks, 
                                    thDt_r[n]->vars->tag_size, 
                                    thDt_r[n]->vars->index_size, 
                                    thDt_r[n]->vars->total_indices, 
                                    thDt_r[n]->vars->overhead_memory_size, 
                                    thDt_r[n]->vars->implementation_memory_size);
                                fprintf(csvFile, ", %d, %d, %d, %d, %d", //statistics
                                    thDt_r[n]->resDt->totalCacheAccesses, 
                                    thDt_r[n]->resDt->cacheHits, 
                                    thDt_r[n]->resDt->totalCacheAccesses - thDt_r[n]->resDt->cacheHits, 
                                    thDt_r[n]->resDt->compulsoryMisses, 
                                    thDt_r[n]->resDt->conflictMisses);
                                fprintf(csvFile, ", %.1f, %.1f, %.1f", //results
                                    thDt_r[n]->vars->cache_hit_rate, 
                                    100 - thDt_r[n]->vars->cache_hit_rate, 
                                    thDt_r[n]->vars->cpi);
                                fflush(csvFile);

                                //free dynamically allocated memory
                                free(thDt_r[n]->resDt->numBlkAcsCntArry);
                                free(thDt_r[n]->resDt);
                                free(thDt_r[n]->args->trace_file_name);
                                free(thDt_r[n]->args);
                                free(thDt_r[n]->vars);
                                free(thDt_r[n]);
                            }
                            //reset counter for next set of threads
                            remainingThreads = numThreads;
                        }

                    }
                }
            }
        }
    }

    fclose(csvFile);
    return 0;
}

void* programThread(void* threadData) {
    threadDataBlock* thDt_t = (threadDataBlock*) threadData; //for thread's thread data

    //print output report
    printf("Trace File: %s\n", thDt_t->args->trace_file_name);
    printf("Cache Size: %d KB\n", thDt_t->args->cache_size);
    printf("Block Size: %d bytes\n", thDt_t->args->block_size);
    printf("Associativity: %d\n", thDt_t->args->associativity);
    printf("Policy: %s\n\n", thDt_t->args->replacement_policy);

    printf("----- Calculated Value -----\n");
    printf("Total #Blocks: %d (2^%d)\n", thDt_t->vars->total_blocks, (int) round(log(thDt_t->vars->total_blocks)/log(2)) );
    printf("Tag Size: %d bits\n", thDt_t->vars->tag_size);
    printf("Index Size: %d bits, Total Indices: %d\n", thDt_t->vars->index_size, thDt_t->vars->total_indices);
    printf("Overhead Memory Size: %d bytes (or %d KB), Implementation Memory Size: %d (or %d KB)\n\n", 
        thDt_t->vars->overhead_memory_size, thDt_t->vars->overhead_memory_size/1024, 
        thDt_t->vars->implementation_memory_size, thDt_t->vars->implementation_memory_size/1024);

    //read in file
    Queue* fileContents = readFile(thDt_t->args->trace_file_name); //queue consists of (char*) void* to each line

    //process file
    Queue* traceData = convertData(fileContents);

    //run simulation
    thDt_t->resDt = runSimulation(traceData, thDt_t->args, thDt_t->vars);
    
    //continue printing output report
    printf("----- Simulation Statistics -----\n");
    printf("Total Cache Accesses:   %d\n", thDt_t->resDt->totalCacheAccesses);
    printf("Cache Hits:             %d\n", thDt_t->resDt->cacheHits);
    printf("Cache Misses:           %d\n", thDt_t->resDt->totalCacheAccesses - thDt_t->resDt->cacheHits);
    printf("--- Compulsory Misses:  %d\n", thDt_t->resDt->compulsoryMisses);
    printf("--- Conflict Misses:    %d\n\n", thDt_t->resDt->conflictMisses);

    printf("----- Results -----\n");
    printf("Cache Hit Rate: %.1f %%\n", thDt_t->vars->cache_hit_rate);
    printf("Cache Miss Rate: %.1f %%\n", 100 - thDt_t->vars->cache_hit_rate);
    printf("CPI: %.1f cycles/instruction\n\n", thDt_t->vars->cpi);

    //print divider between runs
    printf("=======================================================================================\n");
    printf("=======================================================================================\n\n");

    return (void*) thDt_t;
}