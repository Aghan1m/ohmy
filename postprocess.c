#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "def.h"

arr* creatListNode(int number) {
    arr* array = (arr*)malloc(sizeof(arr)*number);
    int i;
    for(i=0;i<number;i++){
        array[i].blocknum = -1;
    }
    return array;
}

lab* ListNode(int number, lable* lnode) {    
    lab *node = (lab*)malloc(sizeof(lab));
    strcpy(node->tagname, lnode->tagname);
    node->bt = lnode->bt;
    node->next = NULL;
    arrhead[number].blocknum = number;
    arrhead[number].data = node;
    return node;
}

lab* addLabNode(lab* last, lable* l) {                
    lab* node = (lab*)malloc(sizeof(lab));
    strcpy(node->tagname, l->tagname);
    node->bt = l->bt;
    node->next = NULL;
    last->next = node;
    return node;
}

stacklab* creatLabStack(){
    stacklab *st = (stacklab*)malloc(sizeof(stacklab));
    st->top = -1;
    return st;
}

void Pushlab(stacklab *st, lab *node) { 
    if(st->top == (STACKSIZE * 1000) - 1)
    {
        printf("step3 stack full! push failed.\n");       
        exit(-1);
    }
    st->data[++(st->top)] = node;
}

lab* Poplab(stacklab *st) {  
    if(st->top == -1){
        printf("step3 stack empty! pop failed.\n");               
        exit(-1);
    }
    return st->data[(st->top)--];
}

int postprocessing(arr* listarr, int countblock) {
    stacklab *st = creatLabStack();     
    int i = 0, flag = 0;
    while(i < countblock) {
        if(listarr[i].blocknum != -1)   break;
        i++;
    }
    lab *tmp = listarr[i].data;
    Pushlab(st, tmp);
    if(tmp->next != NULL){
        tmp = tmp->next;
        Pushlab(st, tmp);
    }
    int num = 0;
    while(i<countblock){
        while(i<countblock) {
            if(listarr[i].blocknum != -1)   break;
            i++;
        }
        tmp = listarr[i].data;
        while(tmp != NULL){
            flag = 0;
            if(strcmp(tmp->tagname,st->data[st->top]->tagname) != 0) flag = 1;
            while(flag ==1 && tmp != NULL){
                Pushlab(st, tmp);
                tmp = tmp->next;
            }
            if(tmp != NULL){
                lab *del = Poplab(st);
                free(del);
                tmp = tmp->next;
            }
        }
        i++;
        num++;
    }
    printf("\tthird phase handled %d second phase block!\n\n", num);
    if(st->top == -1)
        return 1;
    else
        return 0;
}

