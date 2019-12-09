#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"

int main(int argc, char ** argv)
{
    pthread_t pt[THREADNUM];
    int result[THREADNUM];
    int readCount, blocknum=0;
    bufferarray *bufhead=NULL,*buf,*tmp;
    FILE *fp;
    //fp = fopen("../test/test.xml","r");
    fp = fopen(argv[1],"r");

    //读取申请第一个缓冲块
    tmp = mallocBuffer();

    //循环读入数据到缓冲区并进行第一阶段的处理
    while((readCount=fread(tmp->buf,sizeof(char),BUFLEN,fp))>0){
        if(blocknum > 0)
        {       //中间的缓存块链表的链接
            buf->next = analizeBlock(tmp, blocknum++, readCount);
            buf = buf->next;
        }else{  //第一个缓冲区作为缓冲区链表头部的处理
            bufhead = analizeBlock(tmp, blocknum++, readCount);
            buf = bufhead;
        }
        tmp = mallocBuffer();
    }
    fclose(fp);
    printf("\n\t第一阶段预处理结束! 划分文件块%d个！\n\n", blocknum);

    //输出测试第一阶段的结果
    bcs *r;
    buf = bufhead;
    while(buf != NULL){
        r = buf->bcsarr;
        printf("开始第%d块\n",buf->bufnum);
        while(r != NULL){
            printf("blocknumber [%d], offset [%d], Tagtype [%s]\n", buf->bufnum, r->offset, printEnum(r->bt));
            r = r->next;
        }
        buf = buf->next;
    }

// ---------------------------------------------


}

//打印相应标签类型
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
