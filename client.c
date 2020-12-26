#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "common.h"
#include "wrap.h"

#define LOCAL_HOST "127.0.0.1"
#define MACHINE_PORT 8082
// #if 
#define fastServer(argc, args, client) \
        buildServer(client, AF_INET, ((argc<2)?LOCAL_HOST:args[1]), MACHINE_PORT)

void usage(char *program,int status) {
    printf("Usage: %s [destination]...\n", program);
    printf("");
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
    
    if (read(client_fd, &package.package_len, 4) == -1) {
        printf("not receive data...\n");
        return -1;
    }

    read(client_fd, package.filename, sizeof(package.filename));
    
    package.file_content = createBufferSize(package.package_len);
    long cnt = 0;
    printf("file length: %ld\n",package.package_len);
    sleep(2);

    unsigned len = 20;
    char *bar = (char *)malloc(sizeof(char) * (len + 1));
    for (int i = 0; i < len + 1; ++i)  {
        bar[i] = '#';
    }
    int tag = -1;
    while (cnt != package.package_len)
    {
        int i = read(client_fd, package.file_content+cnt, 1000);
        if (i>0) {
            cnt +=i;
        }
        int cur = 100.0 - ((package.package_len - cnt) / (package.package_len / 100.0));

        if(cur != tag) {
            printf("progress:[%s]%d%%\r", bar+len-cur/5, cur);
            fflush(stdout); //一定要fflush，否则不会会因为缓冲无法定时输出。
            usleep(100000);
            tag = cur;
        }
    }

    printf("\n");

    
    fp = fopen(package.filename, "w+");
    fwrite(package.file_content, 1, package.package_len, fp);
    fflush(fp);
    close(client_fd);
    free(package.file_content);

    printf("file is save: %s\n", package.filename);
    return 0;
}