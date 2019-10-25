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
    char* nextline;

    while(true) {
        line1 = (char*) strtok_r(fileContents, '\n', &last);
        if(strcmp(line1, "") == 0 || line1 == NULL) {
            free(line1);
            break;
        }
        line2 = (char*) strtok_r(fileContents, '\n', &last);
        line3 = (char*) strtok_r(fileContents, '\n', &last);
        item = (traceItem*) malloc(sizeof(traceItem));
        memset(item, '\0', sizeof(traceItem));
        sscanf(line1, "%*s %s %x", tmpRAW, &item->addrOfInstr);
        tmp[0] = tmpRAW[1];
        tmp[1] = tmpRAW[2];
        tmp[2] = '\0';
        item->lenOfInstr = (int) strtol(tmp, NULL, 16);
        sscanf(line2, "%*s %x %*s %*s %x", &item->dstM, &item->srcM);
        printf("%s\n%d %d %d %d\n\n", tmp, item->lenOfInstr, item->addrOfInstr, item->dstM, item->srcM);
        free(line1);
        free(line2);
        free(line3);
        enqueue(traceData, (void*) item);
    }

    free(fileContents);
    return traceData;
}