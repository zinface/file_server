#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "common.h"
#include "wrap.h"

#define LOCAL_HOST "127.0.0.1"
#define MACHINE_PORT 8082
// #if 
#define fastServer(argc, args, client) \
        buildServer(client, AF_INET, ((argc<2)?LOCAL_HOST:args[1]), MACHINE_PORT)



void usage(char *program,int status) {
    printf("Usage: %s [destination]...\n", program);
    printf(" ");
    exit(status);
}

int main(int argc,char *args[]){
    struct sockaddr_in client = {0};
    struct package_t package = {0};
    int client_fd;
    FILE *fp = NULL;

    fastServer(argc, args, &client);

    client_fd = Socket(AF_INET, SOCK_STREAM, 0);

    connect(client_fd,(struct sockaddr *)&client,sizeof(client));

// -------------------------------------------------------------------------- file header message
// -------------------------------------------------------------------------- receive file name
    if(read(client_fd, package.filename, sizeof(package.filename)) == -1)
    {
        printf("not receive data...\n");
        return -1;
    }
    bool curfexists = (access(package.filename, F_OK) == 0 ? true : false);
    long curfsize = 0;
    if (curfexists) {
        curfsize = getFileLengthForName(package.filename);
        package._start = curfsize;
    }

// -------------------------------------------------------------------------- upload curfsize
    write(client_fd,&package._start, 4);


// -------------------------------------------------------------------------- receive file length
    read(client_fd, &package.package_len, 4);

    fp = fopen(package.filename,"a+");

// -------------------------------------------------------------------------- download 
    
    char buffer[DATA_PART];
    long fsize = package.package_len;
    // 切片分段整(4096)
    int integerSize = fsize / DATA_PART;
    // 切片分段余(4096)
    int remainderSize = fsize % DATA_PART;

    unsigned len = 20;
    char *bar = (char *)malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len + 1; ++i)  {
        bar[i] = '#';
    }
    int tag = -1;

    for (int i = 0; i != integerSize; i++) {
        long cnt = 0;
        while (cnt != DATA_PART)
        {
            cnt+=read(client_fd, buffer + cnt, DATA_PART - (cnt % DATA_PART));
        }
        fseek(fp,i*DATA_PART,SEEK_SET);
        fwrite(buffer, 1, DATA_PART, fp);

        int cur = 100.0 - ((fsize - i*DATA_PART) / (fsize / 100.0));

        if(cur != tag) {
            printf("progress:[%s]%d%%\r", bar+len-cur/5, cur);
            fflush(stdout); //一定要fflush，否则不会会因为缓冲无法定时输出。
            // usleep(100);
            tag = cur;
        }
    }

    long cnt = 0;
    while (cnt != remainderSize)
    {
        cnt+=read(client_fd, buffer + cnt, remainderSize - (cnt % remainderSize));
    }
    fseek(fp,fsize-remainderSize+curfsize,SEEK_SET);
    fwrite(buffer, 1, remainderSize, fp);

    printf("progress:[%s]%d%%\r", bar+len-100/5, 100);
    fflush(stdout); //一定要fflush，否则不会会因为缓冲无法定时输出。
    usleep(100000);

    printf("\n");
    fflush(fp);
    close(client_fd);

    printf("file is save: %s\n", package.filename);
    return 0;
}