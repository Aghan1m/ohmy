#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"

// malloc buffer and init
bufferarray* mallocBuffer() {
    bufferarray *p = (bufferarray*)malloc(sizeof(bufferarray));
    if(p == NULL){
        printf("Allocate memory failed!\n");
        exit(-1);
    }
    p->bcsarr = NULL;
    p->bufnum = -1;
    p->buflen = -1;
    p->START_STAGE1 = 0;
    p->START_STAGE2 = 0;
    p->START_STAGE3 = 0;
    p->FINISH_STAGE1 = 0;
    p->FINISH_STAGE2 = 0;
    p->FINISH_STAGE3 = 0;
    pthread_mutex_init(&(p->mutex), NULL);    
    p->next = NULL;
    return p;
}

int strCmp(char *str,int start,int len,char str2[]) { 
	int i=0;
	for(i=0;i<len;++i)
		if(str[start + i]!=str2[i])     return 0;
	return 1;
}

bcs* bcsnode(Bcstype type,int offset) { 
    bcs *bcslist = (bcs*)malloc(sizeof(bcs));
    if(bcslist == NULL){
        printf("Allocate memory failed!\n");
        exit(-1);
    }
    bcslist->offset = offset;
    bcslist->bt = type;
    bcslist->next = NULL;
    return bcslist;
}

bufferarray* analizeBlock(bufferarray *block, int blocknum, int buflen) {
    bcs *p = NULL;
    int i = 0,j = 0; // j:index of each pair of tags, offset 
    block->START_STAGE1 = 1;                                        // change identifier to 1
    block->bufnum = blocknum;
    block->buflen = buflen;
    while(i<BUFLEN){
        if(block->buf[i++] != '<')  // when not '<' judge next char
			continue;                                    
        if(block->buf[i]!='/' && block->buf[i]!='?' && block->buf[i]!='!') {     // <xxxx 
            if(j != 0) {                                                         // not first
                j = i;
                while(i < BUFLEN){
                    if(block->buf[i] == '/' && block->buf[i+1] == '>'){         // <xxxx/>
                        break;
                    } else if(block->buf[i] == '>') {                             // <xxxx>
                        p->next = bcsnode(Stag_start,j);
                        p = p->next;
                        break;
                    }
                    i++;
                }
            } else {                                                              // first
                j = i;
                while(i < BUFLEN) {
                    if(block->buf[i] == '/' && block->buf[i+1] == '>') {         //<xxxx/>
                        j = 0;
                        break;
                    } else if(block->buf[i] == '>') {                             //</xxxx>
                        block->bcsarr = bcsnode(Stag_start,j);
                        p = block->bcsarr;
                        break;
                    }
                    i++;
                }
            }
        } else if(j == 0) {                                                       // when first char is '<' j=0 is invalid 
            continue;
        } else if(block->buf[i] == '/') {                                         //</xxxx>
            p->next = bcsnode(Etag_start,i);
            p = p->next;
            while(i < BUFLEN) {
                if(block->buf[i++] == '>')              
					break;
            }
        } else if(block->buf[i] == '?') {                                         //<?xxxx?>
            p->next = bcsnode(PI_start,i);
            p = p->next;
            while(i < BUFLEN){
                if(strCmp(block->buf,i++,2,"?>"))       
					break;
            }
        } else if(strCmp(block->buf,i,3,"!--")) {                                 //<!--xxxx-->
            p->next = bcsnode(COMMENT_start,i);
            p = p->next;
            while(i < BUFLEN){
                if(strCmp(block->buf,i++,3,"-->"))      
					break;
            }
        } else if(strCmp(block->buf,i,8,"![CDATA[")) {                            //<![CDATA［xxxx]]>
            p->next = bcsnode(CDSECT_start,i);
            p = p->next;
            while(i < BUFLEN){
                if(strCmp(block->buf,i++,3,"]]>"))      break;
            }
        }
        i++;
    }
    block->FINISH_STAGE1 = 1;                        
    block->START_STAGE1 = 0;                        
    return block;
}
