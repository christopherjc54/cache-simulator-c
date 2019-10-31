#include "linked_lists.h"

LinkedList *createLinkedList() {
    LinkedList *list = (LinkedList*) malloc(sizeof(LinkedList));
    if(list != NULL) {
        list->pFirst = NULL;
        list->length = 0;
    }
    return list;
}

void freeLinkedList(LinkedList *list) {
    free(list);
}

LLType getFirst(LinkedList *list) {
    if(isEmpty(list)) {
        return NULL;
    }
    return list->pFirst->data;
}

LLType getItem(LinkedList *list, int itemNumber) {
    if(isEmpty(list)) {
        return NULL;
    }
    int i;
    LLNode* node = list->pFirst;
    for(i=0; i < itemNumber; i++) {
        node = node->pNext;
        if(node == NULL) {
            printf("Item number #%d not found in list.\n", itemNumber);
            return -1;
        }
    }
    return node->data;
}

void addItem(LinkedList *list, LLType item) {
    LLNode *node = (LLNode*) malloc(sizeof(LLNode));
    if(node==NULL) {
        printf("Failed to allocate memory.\n");
        exit(-1);
    }

    node->pNext=NULL;
    node->data = item;
    if(isEmpty(list)) {
        list->pFirst = node;
        list->length++;
        return;
    }
}

bool isEmpty(LinkedList *list)
{
    if(list->pFirst == NULL && list->length == 0) {
        return true;
    } else {
        return false;
    }
}