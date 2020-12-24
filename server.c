#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include "server.h"
#include "wrap.h"

void buildServer(struct sockaddr_in* servaddr, sa_family_t family, in_port_t port, char* address){
    servaddr->sin_family =  family;
    servaddr->sin_port = htons(port);
    servaddr->sin_addr.s_addr = inet_addr(address);
}
// --------------------------------------------------------------------------
int buildServerListen(char* address, int port){
    struct sockaddr_in servaddr = {0};
    int listenfd;

    listenfd = Socket(AF_INET,SOCK_STREAM,0);
    buildServer(&servaddr, AF_INET, port, address);
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd,4);
    return listenfd;
}

int buildClientResponse(int server_fd){
    struct sockaddr_in cliaddr = {0};
    socklen_t cliaddr_len;
    cliaddr_len = sizeof(cliaddr);
    int client_fd;

    buildServer(&cliaddr, AF_INET, 0, LISTEN_ADDR);
    
    client_fd = Accept(server_fd,(struct sockaddr*)&cliaddr, &cliaddr_len);
    return client_fd;
}

// --------------------------------------------------------------------------file/buffer
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
char *createBufferSize(long _msize){
    char* m = malloc(_msize);
    memset(m,0,_msize);
    return m;
}
void buildBufferFromFile(char* buffer,long _fsize, FILE *_fp){
    buffer = createBufferSize(_fsize);
    rewind(_fp);
    fread(buffer,1,_fsize,_fp);
}
char *buildBufferFromFileName(char *filename){
    FILE *fp = NULL;
    char *buffer = NULL;
    long file_size;

    if((fp = fopen(filename,"r")) == NULL){
        printf("获取文件失败\n");
        return NULL;
    }
    
    file_size = getFileLength(fp);
    buildBufferFromFile(&buffer, file_size, fp);
    fclose(fp);
    return buffer;
}

// -------------------------------------------------------------------------- package
void buildPackage(struct package_t *package, char* filename){
    package->package_len = 4 + strlen(filename)+1 + getFileLengthForName(filename);
    package->filename = filename;
    package->file_content = buildBufferFromFileName(filename);
}
void sendPackage(int client_fd, struct package_t *package, char* filename) {
    write(client_fd,&package->package_len, 4);
    write(client_fd,package->filename, strlen(filename)+1);
    write(client_fd,package->file_content, getFileLengthForName(filename));
}
// --------------------------------------------------------------------------
int main(int argc,char *args[]){
    struct package_t package = {0};
    char *filename = "test.txt";
    int listenfd, client_fd;

    listenfd = buildServerListen(LISTEN_ADDR, LISTEN_PORT);
    client_fd= buildClientResponse(listenfd);

    buildPackage(&package, filename);
    sendPackage(client_fd, &package, filename);

    close(client_fd);
}