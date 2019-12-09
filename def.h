#include <pthread.h>

#define BUFLEN 10240                    // buffer length 
#define APPEND 512                      //填充字符区大小
#define NAMELEN 256                     //标签名字最大长度
#define STACKSIZE 16                    //栈的深度
#define THREADNUM 10                    //第二阶段解析并行的线程数

typedef enum Bcstype_t {  
    Stag_start,
    Etag_start,
    PI_start,
    Content,
    CDSECT_start,
    COMMENT_start
} Bcstype;

typedef struct bcs_t {                   //标签节点的数据结构
    int offset;                         //相对于该数据块的偏移
    int taglen;                         //该标签的长度（计算结束位置）
    Bcstype bt;                         //该标签的类型
    struct bcs_t *next;
} bcs;

typedef struct bufferarry_t{            //缓冲区块的数据结构
    char buf[BUFLEN + APPEND];          //存放读入的数据
    bcs *bcsarr;                        //识别标签后存放结果的链表
    int bufnum;                         //缓冲块块号
    int buflen;                         //缓冲区所存数据长度

    pthread_mutex_t mutex;              //互斥锁

    int START_STAGE1;                   //第1阶段开始处理该块时置1
    int FINISH_STAGE1;                  //第1阶段处理完置1

    int START_STAGE2;                   //第2阶段开始处理该块时置1
    int FINISH_STAGE2;                  //第2阶段处理完置1

    int START_STAGE3;                   //第3阶段开始处理该块时置1
    int FINISH_STAGE3;                  //第3阶段处理完置1

    struct bufferarry_t *next;          //下一个缓冲区块指针
} bufferarray;


char* printEnum(Bcstype type);                                                      //main.c

//申请缓冲区并初始化
bufferarray* mallocBuffer();                                                        //step1.c
//字符串比较
int strCmp(char *str,int start,int len,char str2[]);                                //step1.c
//创建标记信息链表
bcs* bcsnode(Bcstype type,int offset);                                              //step1.c
//分析数据块，找到个标签的位置并存入链表中
bufferarray* analizeBlock(bufferarray *block, int blocknum, int buflen);            //step1.c


