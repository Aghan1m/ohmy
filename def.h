#include <pthread.h>

#define BUFLEN 10240                    // buffer length 
#define APPEND 512                      // append char size
#define NAMELEN 256                     // maxlength of tag
#define STACKSIZE 16                    // depth of stack
#define THREADNUM 10                    // concurrent thread number(for parsing xml)

// boundary character/string type
typedef enum Bcstype_t {  
    Stag_start,
    Etag_start,
    PI_start,     // Process Instruction 
    Content,      // data
    CDSECT_start, // CDATA
    COMMENT_start
} Bcstype;

typedef struct bcs_t {                   
    int offset;                         // offset of the data block
    int taglen;                         // length of the tag
    Bcstype bt;                         // bcstype
    struct bcs_t *next;                 // linked list of bcs
} bcs;

typedef struct bufferarry_t {           // array of the buffers
    char buf[BUFLEN + APPEND];          // save read data
    bcs *bcsarr;                        // after preprocessing, save the result
    int bufnum;                         // buffer number
    int buflen;                         // buffer length

    pthread_mutex_t mutex;              

    int START_STAGE1;                   // set to 1 when start preprocessing
    int FINISH_STAGE1;                  // set to 1 when finish preprocessing

    int START_STAGE2;                   // set to 1 when start processing
    int FINISH_STAGE2;                  // set to 1 when finish processing

    int START_STAGE3;                   // set to 1 when start postprocessing
    int FINISH_STAGE3;                  // set to 1 when finish postprocessing

    struct bufferarry_t *next;          // next buffer pointer
} bufferarray;

/* 
 * main
 */
char* printEnum(Bcstype type);

/* 
 * preprocessing
 */
bufferarray* mallocBuffer();  // allocate and init buffer

int strCmp(char *str,int start,int len,char str2[]); // compare string

bcs* bcsnode(Bcstype type,int offset);    // create bcs node

bufferarray* analizeBlock(bufferarray *block, int blocknum, int buflen);            // analysis and save to linkedlist

/* 
 * processing
 */
