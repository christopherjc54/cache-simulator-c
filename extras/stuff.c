/* function prototyptes
    //alternative method to get data
    char* readFileToString(char* fileName);
    Queue* convertDataFromStr(char* fileContents);
*/

/* code to use them
    //alternative method to read in data
    char* fileContentsStr = readFileToString(args->trace_file_name);
    Queue* traceData = convertDataFromStr(fileContentsStr);
*/

//reads in entire file into one string
char* readFileToString(char* fileName) {
    FILE* file;
    char* contents;
    long numbytes;
    
    if((file = fopen(fileName, "r")) == NULL) return (char*) -1;
    fseek(file, 0, SEEK_END);
    numbytes = ftell(file);
    fseek(file, 0, SEEK_SET);
    contents = (char*) calloc(numbytes, sizeof(char));
    if(contents == NULL) return (char*) -1;
    fread(contents, sizeof(char), numbytes, file);
    fclose(file);

    return contents;
}

Queue* convertDataFromStr(char* fileContents) {
    Queue* traceData = createQueue();
    char *line1, *line2, *line3;
    traceItem *item;
    char tmpRAW[6], tmp[3];

    if(fileContents == NULL) return -1;
    char* last;

    for(line1 = strtok_r(fileContents, "\n", &last); line1 != NULL && strcmp(line1, "") != 0; line1 = strtok_r(NULL, "\n", &last)) {
        line2 = strtok_r(NULL, "\n", &last);
        line3 = strtok_r(NULL, "\n", &last);
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
        if(line1 != NULL) free(line1);
        if(line2 != NULL) free(line2);
        if(line3 != NULL) free(line3);
        enqueue(traceData, (void*) item);
    }

    free(fileContents);
    return traceData;
}