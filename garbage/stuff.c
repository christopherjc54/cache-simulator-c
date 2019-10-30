/*
    char* fileContentsStr = readFileToString(trace_file_name);
    Queue* traceData = convertDataFromStr(fileContentsStr);
    free(fileContentsStr);
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
    char *tmpRAW, tmp[3];

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
        tmpRAW = (char*) malloc(10); //only need 6
        if(tmpRAW == NULL) {
            printf("failed to malloc tmpRAW\n");
            exit(-1);
        }
        sscanf(line1, "%*s %s %x", tmpRAW, &item->addrOfInstr);
        tmp[0] = tmpRAW[1];
        tmp[1] = tmpRAW[2];
        tmp[2] = '\0';
        free(tmpRAW);
        item->lenOfInstr = (int) strtol(tmp, NULL, 16);
        sscanf(line2, "%*s %x %*s %*s %x", &item->dstM, &item->srcM);
        enqueue(traceData, (void*) item);
    }

    return traceData;
}