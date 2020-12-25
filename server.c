#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>

#include "common.h"
#include "server.h"
#include "wrap.h"


// -------------------------------------------------------------------------- socket/server

int buildServerListen(char* address, int port){
    struct sockaddr_in servaddr = {0};
    int listenfd;
    int on = 1;

    listenfd = Socket(AF_INET,SOCK_STREAM,0);
    buildServer(&servaddr, AF_INET, address, port);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    Listen(listenfd,4);
    return listenfd;
}

int buildClientResponse(int server_fd){
    struct sockaddr_in cliaddr = {0};
    socklen_t cliaddr_len;
    cliaddr_len = sizeof(cliaddr);
    int client_fd;

    buildServer(&cliaddr, AF_INET, LISTEN_ADDR, 0);
    
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

void buildBufferFromFile(char **buffer, long _fsize, FILE *_fp){
    *buffer = createBufferSize(_fsize);
    rewind(_fp);
    fread(*buffer, 1, _fsize, _fp);
}

char *buildBufferFromFileName(char *filename){
    FILE *fp = NULL;
    void *buffer = NULL;
    long file_size;

    if((fp = fopen(filename,"r")) == NULL){
        printf("获取文件失败\n");
        return NULL;
    }
    
    file_size = getFileLength(fp);
    buildBufferFromFile(&buffer, file_size, fp);   // buffer = char *buffer    指针传;      char *buffer 收  / buffer 改
                                                   // &buffer = char* *buffer  指针地址传    char **buffer 收 / *buffer 改
    fclose(fp);
    return buffer;
}

// -------------------------------------------------------------------------- package
void buildPackage(struct package_t *package, char* filename){
    package->package_len = getFileLengthForName(filename);
    memcpy(package->filename, filename, strlen(filename));
    package->file_content = buildBufferFromFileName(filename);
}

void sendPackage(int client_fd, struct package_t *package, char* filename) {
    write(client_fd, &package->package_len, 4);
    write(client_fd, package->filename, sizeof(package->filename));
    write(client_fd, package->file_content, getFileLengthForName(filename));
}

// -------------------------------------------------------------------------- main
int main(int argc,char *args[]){
    struct package_t package = {0};
    char *filename = "test.txt";
    int listenfd, client_fd;
    filename, getFileLengthForName(filename);

    listenfd = buildServerListen(LISTEN_ADDR, LISTEN_PORT);
    client_fd= buildClientResponse(listenfd);

    buildPackage(&package, filename);
    sendPackage(client_fd, &package, filename);

    close(client_fd);
}