#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "def.h"

#define MILLION 1000000

pthread_t pt[THREADNUM];
int result[THREADNUM];
int readCount, blocknum=0;
bufferarray *bufhead = NULL;
bufferarray *buf, *tmp;
FILE *fp;
	
char* printBcstype(Bcstype type) {
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
    printf("\t xml file analysis done, numbers of concurrent thread is %d\n", THREADNUM);
}

void step_three() {

}

int main(int argc, char ** argv) {
	struct timespec start;
	struct timespec end;
	long time_step1;
	long time_step2;
	long time_step3;
	long timediff;

    //fp = fopen("../test/test.xml","r");
    fp = fopen(argv[1],"r");
    
	clock_gettime(CLOCK_MONOTONIC, &start);	
	step_one();
	clock_gettime(CLOCK_MONOTONIC, &end);	
	time_step1 = MILLION*(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &start);	
	step_two();
	clock_gettime(CLOCK_MONOTONIC, &end);	
	time_step2 = MILLION*(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &start);	
	step_three();
	clock_gettime(CLOCK_MONOTONIC, &end);	
	time_step3 = MILLION*(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1000;

	timediff = time_step1 + time_step2 + time_step3;
	fprintf(stdout, "step1 took %ld ms\n", time_step1/1000);
	fprintf(stdout, "step2 took %ld ms\n", time_step2/1000);
	fprintf(stdout, "step3 took %ld ms\n", time_step3/1000);
	fprintf(stdout, "total took %ld ms\n", timediff/1000);
}


