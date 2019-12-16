#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"

pthread_t pt[THREADNUM];
int result[THREADNUM];
int readCount, blocknum=0;
bufferarray *bufhead = NULL;
bufferarray *buf, *tmp;
FILE *fp;
	
char* printEnum(Bcstype type) {
    switch(type) {
        case Stag_start:
            return "Stag";
        case Etag_start:
            return "Etag";
        case PI_start:
            return "PI";
        case Content:
            return "content";
        case CDSECT_start:
            return "CDATA";
        case COMMENT_start:
            return "comment";
        default:
            return "Tag";
    }
}

// parse each block of xml
void step_one() {
	// first buffer
    tmp = mallocBuffer();

    // read data to buffer and analyse
    while((readCount = fread(tmp->buf, sizeof(char), BUFLEN, fp)) > 0) {
        if(blocknum > 0) {
            buf->next = analizeBlock(tmp, blocknum++, readCount);
            buf = buf->next;
        } else{  // handle first buffer
            bufhead = analizeBlock(tmp, blocknum++, readCount);
            buf = bufhead;
        }
        tmp = mallocBuffer();
    }
    fclose(fp);
    printf("\n\tThe first preprocessing work done! Divide data into %d blocks！\n\n", blocknum);
}

// multithread handle blocks
void step_two() {
    int i;
    buf = bufhead;
    for(i=0; i<THREADNUM; i++) {
		// TODO result
        result[i] = pthread_create(&(pt[i]), NULL, processing, (void*)buf);
        buf = buf->next;
        if(result[i] != 0){
            printf("create pthread error!\n");
            exit(-1);
        }
    }
	// concurrent execute
    for(i=0; i<THREADNUM; i++) { 
        pthread_join(pt[i], NULL);
    }
    printf("\t第二阶段多线程处理完毕！ 并行线程数为%d个!\n\n", THREADNUM);
}

void step_three() {

}

int main(int argc, char ** argv) {
    fp = fopen("../test/test.xml","r");
    //fp = fopen(argv[1],"r");
	step_one();
	step_two();
	step_three();
}


