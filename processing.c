#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"

lable* creatData(char *tagName, Bcstype bt) { 
    lable *node = (lable*)malloc(sizeof(lable));
    strcpy(node->tagname, tagName);
    node->bt = bt;
    return node;
}

stack* creatStack() { 
    stack *st = (stack*)malloc(sizeof(stack));
    st->top = -1;
    return st;
}

int Push(stack *st, lable *node) {
    if(st->top == STACKSIZE-1) {
        printf("full stack, push failed!\n"); 
        exit(-1);
    }
    st->data[++(st->top)] = node;
    return 1;
}

lable* Pop(stack *st) {
    if(st->top == -1) {
        printf("empty stack, pop failed!\n");
        exit(-1);
    }
    return st->data[(st->top)--];
}

char* getname(char* strarr, char* str, char ch) { 
    int i;
    for(i=0;i<NAMELEN && str[i]!=ch && str[i]!=' ';i++) 
		strarr[i] = str[i];
    strarr[i] = '\0';
    return strarr;
}

char* getTagName(char* strarr, char* str, Bcstype bt) {  
//	printf("getTagName bt:%d\n", bt);
    switch(bt){
        case Stag_start:
            getname(strarr, str,'>');
            break;
        case Etag_start:
            getname(strarr, str+1,'>');
            break;
/*  //
        case PI_start:

            break;
        case Content:

            break;
        case CDSECT_start:

            break;
        case COMMENT_start:

            break;
*/
        default:
            strarr[0] = '\0';
            break;
    }
    return strarr;
}

void stackInOut(bufferarray *block, char* loc, Bcstype bt, stack* st) {
    char *str = (char*)malloc(NAMELEN * sizeof(char));
    strcpy(str, getTagName(str, loc, bt));
    if(st->top>-1 && strcmp(str, st->data[st->top]->tagname) == 0) { // stack not empty and match the top elem
//        printf("block number:%d\ttop element before pop stack:%s\tsize of current stack:%d\n", block->bufnum, st->data[st->top]->tagname, st->top+1);
        lable *delnode = Pop(st);
        free(delnode);
        free(str);
    } else {  // empty stack or top element not matching
        if(str[0] == '\0')  
			return;
        lable* node = creatData(str, bt);
        Push(st, node);
 //       printf("block number:%d\ttop element after push stack:%s\tsize of current stack%d\n", block->bufnum, str,st->top+1);
    }
}

void stackMatching(bufferarray *block) { 
    if(block == NULL)   
		return;
    bcs *r = block->bcsarr;
    stack *st = creatStack();
    char *name = (char*)malloc(NAMELEN * sizeof(char));
    strcpy(name, getTagName(name, &(block->buf[r->offset]), r->bt));
	// first block analysis
    lable* node = creatData(name, r->bt);
    Push(st, node);
    r = r->next;

    while(r != NULL && r->next != NULL) {
        stackInOut(block, &(block->buf[r->offset]), r->bt, st);
        r = r->next;
    }

    if(block->next != NULL) {
        int i, j;
        j = block->next->bcsarr->offset;
        if(r->bt == Stag_start) { 
            printf("last char of the block:%s\n",&(block->buf[block->buflen-1]));
            strncat(block->buf,block->next->buf,j-1);
            block->buflen = block->buflen + j-1;
            printf("append next buffer's content:%s\n",&(block->buf[block->buflen-j+1]));
        } else {
            if(block->buf[block->buflen-1] != '>') {
                printf("content after last tag:%s\n",&(block->buf[r->offset])); 
                strncat(block->buf,block->next->buf,j-1);
                printf("after append next buffer's content:%s\n",&(block->buf[r->offset-1]));
                block->buflen = block->buflen + j-1;
            }
        }

// analyse tags
        i = r->offset;
        j = block->next->bcsarr->offset;

        int k, flag = 0;
        while(i < block->buflen) {                           // last tag of this block
            if(block->buf[i++] != '<')    
				continue;
            if(block->buf[i]!='/'&&block->buf[i]!='?'&&block->buf[i]!='!') {             //<xxxx/> filter
                flag = 0;
                k = i;
                while(k < block->buflen) {
                    if(strCmp(&(block->buf[k++]), 0, 2, "/>")) {
                        i = k+1;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1) {
                    i++;
                    continue;
                }
            }

            if(block->buf[i]!='/'&&block->buf[i]!='?'&&block->buf[i]!='!') {             //<xxxx>
                stackInOut(block, &(block->buf[i]), Stag_start, st);
                continue;
            } else if(block->buf[i]!='/') { 
                i++;
                continue;
            } else {                                                                      //</xxxx>
                stackInOut(block, &(block->buf[i]), Etag_start, st);
            }
        }
    }

    pthread_mutex_lock(&(block->mutex));
    block->START_STAGE2 = 0;
    block->FINISH_STAGE2 = 1;
    pthread_mutex_unlock(&(block->mutex)); 
    if(st->top != -1) {                                                          // phase 3
/*
        list* tmp = addListNode(block->bufnum, Pop(st));
        pthread_mutex_lock(&mutex);
        point->next = tmp;
        point = point->next;
        pthread_mutex_unlock(&mutex);
        lab *labnode = point->data;
        while(st->top > -1){
            labnode->next = addLabNode(Pop(st));
            labnode = labnode->next;
        }// */

        printf("third phase: block number%d\t not empty stack! size of the stack:%d\n", block->bufnum, st->top+1);//st->data[st->top]->tagname);
        lab* tmp = ListNode(block->bufnum, st->data[0]);
        int m = 1;
        while(m <= st->top){
            tmp = addLabNode(tmp,st->data[m++]);
        }
        while(st->top > -1){
            node = Pop(st);
            free(node);
        }
    } else {
        printf("block buffer number:%d\tempty stack.\n", block->bufnum);
    }
    free(st);
}

void* processing(void* arg) {
    bufferarray *block = (bufferarray*)arg;
    int flag1, flag2;
    while(block != NULL){
        flag1 = 0;

        pthread_mutex_lock(&(block->mutex));
        if(block->START_STAGE2 == 0 && block->FINISH_STAGE2 == 0) {
            block->START_STAGE2 = 1;
            flag1 = 1;
        }
        pthread_mutex_unlock(&(block->mutex));
        if(flag1){
            stackMatching(block);
        }

        flag2 = 1;
        while(flag2 && block != NULL) {
            pthread_mutex_lock(&(block->mutex));
            if(block->START_STAGE2 == 0 && block->FINISH_STAGE2 == 0)   
				flag2 = 0;     
            pthread_mutex_unlock(&(block->mutex));
            block = block->next;
        }

    }
    return (void*)NULL;
}
