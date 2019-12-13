#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"

int main(int argc, char ** argv)
{
    pthread_t pt[THREADNUM];
    int result[THREADNUM];
    int readCount, blocknum=0;
    bufferarray *bufhead = NULL;
    bufferarray *buf, *tmp;
    FILE *fp;
	
	// open xml file
    // fp = fopen("../test/test.xml","r");
    fp = fopen(argv[1],"r");

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

// test preprocessing
//    bcs *r;
//    buf = bufhead;
//    while(buf != NULL){
//        r = buf->bcsarr;
//        printf("start block nums:%d\n",buf->bufnum);
//        while(r != NULL){
//            printf("blocknumber [%d], offset [%d], Tagtype [%s]\n", buf->bufnum, r->offset, printEnum(r->bt));
//            r = r->next;
//        }
//        buf = buf->next;
//    }

// ---------------------------------------------


}

// print tag
char* printEnum(Bcstype type){
    switch(type){
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
