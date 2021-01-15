#include "common.h"
#include <string.h>

// -------------------------------------------------------------------------- 

void buildServer(struct sockaddr_in* servaddr, sa_family_t family, char* address , in_port_t port){
    servaddr->sin_family =  family;
    servaddr->sin_port = htons(port);
    servaddr->sin_addr.s_addr = inet_addr(address);
}

// -------------------------------------------------------------------------- 
char *createBufferSize(long _msize){
    char* m = (char *)malloc(_msize);
    memset(m, 0, _msize);
    return m;
}

// -------------------------------------------------------------------------- file length func
long getFileLength(FILE *_fp){
    fseek(_fp,0,SEEK_END);
    return ftell(_fp);
}

long getFileLengthForName(char *filename){
    FILE *fp = NULL;
    long file_size;
    
    if((fp = fopen(filename,"r")) == NULL){
        printf("打开文件失败\n");
        return -1;
    }
    file_size = getFileLength(fp);
    
    fclose(fp);
    return file_size;
}