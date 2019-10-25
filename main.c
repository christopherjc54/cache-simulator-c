#include "main.h"

void handleIncorrectUsage() {
    printf("\nInvalid number of arguments. Please launch using\nSim.exe -f trace1.txt -s 1024 -b 16 -a 2 -r RR\n\n-f <trace file name> [ name of text file with the trace ]\n-s <cache size in KB> [ 1 KB to 8 MB ]\n-b <block size> [ 4 bytes to 64 bytes ]\n-a <associativity> [ 1, 2, 4, 8, 16 ]\n-r <replacement policy> [ RR, RND, or LRU ]\n\n");
    exit(-1);
}

//returns queue of each line in void* form (cast to char* when using)
Queue* readFile(char* fileName) {
    Queue* fileContents = createQueue();
    char* m_line;

    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read; //ssize_t

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        printf("Error opening %s\n", fileName);
        exit(-1);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        m_line = malloc(strlen(line));
        if(m_line == NULL) {
            printf("failed to malloc string for next line of file\n");
            exit(-1);
        }
        strcpy(m_line, line);
        enqueue(fileContents, (void*) m_line); //cast back to char* when using line
    }

    fclose(fp);
    if(line) free(line);
    return fileContents;
}

Queue* convertData(Queue* fileContents) {
    Queue* traceData = createQueue();
    char *line1, *line2, *line3;
    traceItem *item;
    char *tmpRAW, tmp[3];

    while(!isEmpty(fileContents)) {
        line1 = (char*) dequeue(fileContents);
        if(strcmp(line1, "") == 0) {
            free(line1);
            break;
        }
        line2 = (char*) dequeue(fileContents);
        line3 = (char*) dequeue(fileContents);
        item = (traceItem*) malloc(sizeof(traceItem));
        if(item == NULL) {
            printf("failed to malloc trace item\n");
            exit(-1);
        }
        sscanf(line1, "%*s %s %x", tmpRAW, &item->addrOfInstr);
        tmp[0] = tmpRAW[1];
        tmp[1] = tmpRAW[2];
        tmp[2] = '\0';
        item->lenOfInstr = (int) strtol(tmp, NULL, 16);
        sscanf(line2, "%*s %x %*s %*s %x", &item->dstM, &item->srcM);
        free(line1);
        free(line2);
        free(line3);
        enqueue(traceData, (void*) item);
    }

    free(fileContents);
    return traceData;
}

int main(int argc, char* argv[]) {
    //check for correct usage
    if(argc % 2 == 0 || argc < 3 || argc > 11) {
        handleIncorrectUsage();
    }

    //initialize argument variables
    char* trace_file_name = NULL;
    int cache_size = NULL; //in KB
    int block_size = NULL; //in bytes
    int associativity = NULL;
    char* replacement_policy = NULL; //will be RR, RND, or LRU

    //read in arguments
    int i;
    for(i=1; i < argc; i+=2 ) {
        if(strcmp(argv[i], "-f") == 0) {
            trace_file_name = argv[i+1];
        } else if(strcmp(argv[i], "-s") == 0) {
            cache_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-b") == 0) {
            block_size = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-a") == 0) {
            associativity = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-r") == 0) {
            replacement_policy = argv[i+1];
        }
    }

    //set defaults if needed
    //TODO: comment out printf's and add (default) after variables in input report :(
    bool usedDefault = false;
    if(trace_file_name == NULL) handleIncorrectUsage();
    if(cache_size == NULL) {
        cache_size = 1024;
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Cache size not specified, using default of 1024KB.\n");
    }
    if(block_size == NULL) {
        block_size = 16;
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Block size not specified, using default of 16 bytes.\n");
    }
    if(associativity == NULL) {
        associativity = 2;
        if(!usedDefault) printf("\n");
        usedDefault = true;
        printf("Associativity not specified, using default of 2.\n");
    }
    if(replacement_policy == NULL) {
        replacement_policy = "RR";
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
    printf("\nTrace File: %s\nCache Size: %d\nBlock Size: %d\nAssociativity: %d\nR-Policy: %s\n\n", trace_file_name, cache_size, block_size, associativity, replacement_policy);
    
    //read in file
    Queue* fileContents = readFile(trace_file_name); //queue consists of (char*) void* to each line

    //process file
    //printFileContents_dummySimulation(fileContents); //prints file contents
    Queue* traceData = convertData(fileContents);
    printTraceData_dummySimulation(traceData);

    //initialize variables for output report
    int total_blocks = ((cache_size*1024) / block_size) / 1024; //in KB
    int index_size = round ( log10( (cache_size*1024) / (associativity * block_size) )/log10(2) ); //bits
    int offset_size = round( log10(block_size)/log10(2) ); //bits
    int tag_size = 32 - offset_size - index_size; //bits
    int total_indices = pow(2, index_size) / 1024; //in KB
    //TODO: figure out how we're calculating overhead & implementation memory sizes
    int overhead_memory_size = ( (1+tag_size)*associativity * total_indices ) / 8; //in bytes
    int implementation_memory_size = -1; //in bytes

    double cache_hit_rate = -1; //percent
    double cpi = -1; //cycles per instruction

    //print output report
    printf("Cache Size: %d KB\nBlock Size: %d bytes\nAssociativity: %d\nPolicy: %s\n\n", cache_size, block_size, associativity, replacement_policy);
    printf("----- Calculated Value -----\nTotal #Blocks: %d KB (2^%d)\nTag Size: %d bits\nIndex Size: %d bits, Total Indices: %d KB\nOverhead Memory Size: %d bytes (or %d KB), Implementation Memory Size: %d (or %d KB)\n\n", 
        total_blocks, (int) round(log10(total_blocks*1024)/log10(2)), tag_size, index_size, total_indices, overhead_memory_size*1024, overhead_memory_size, implementation_memory_size, implementation_memory_size/1024);
    printf("----- Results -----\nCache Hit Rate: %.1f %%\nCPI: %.1f cycles/instruction\n\n", cache_hit_rate, cpi);

    return 0;
}