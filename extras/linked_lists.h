/*
    Author: Jessica Sherette
    Modified and updated by Christopher Coutinho
*/

#ifndef _linked_lists_h
#define _linked_lists_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//set LLType here (i.e. char*)
#define LLType void*

typedef struct LLNode {
    LLType data;            //LLType data contained in this node
    struct LLNode *pNext;   //pointer to the next node in the linked list
}  LLNode;

typedef struct LinkedList {
    LLNode *pFirst;         //pointer to the first element of the linked list
    int length;
} LinkedList;

LinkedList *createLinkedList();
void freeLinkedList(LinkedList *list); //does NOT free data contents

LLType getFirst(LinkedList *list);
LLType getItem(LinkedList *list, int itemNumber);
void addItem(LinkedList *list, LLType item);

bool isEmpty(LinkedList *list);

#endif