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
int server_listen(){
    struct sockaddr_in server = {0};
    server.sin_family =  AF_INET;
    server.sin_port = htons(LISTEN_PORT);
    server.sin_addr.s_addr = inet_addr(LISTEN_ADDR);
    int server_fd = -1;
    if( (server_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        printf("socket create error\n");
        exit(-1);
    }
    printf("socket create ok\n");
    if(bind(server_fd,(struct sockaddr *)&server,sizeof(server)) == -1){
        printf("socket bind failue\n");
        exit(-1);
    }
    printf("socket bind ok\n");
    
    if(listen(server_fd,4) == -1){
        printf("listen error\n");
    }
    printf("listen ok\n");
    return server_fd;
}
int accept_request(int server_fd){
    struct sockaddr_in tmp_sock = {0};
    tmp_sock.sin_family = AF_INET;
    tmp_sock.sin_port = 0; //绑定随机端口
    tmp_sock.sin_addr.s_addr = inet_addr(LISTEN_ADDR);
    socklen_t tmp_sock_len = sizeof(tmp_sock);
    int client_fd = accept(server_fd,(struct sockaddr*)&tmp_sock,&tmp_sock_len);
    if(client_fd == -1){
        printf("accept error\n");
    }
    printf("accept ok\n");
    return client_fd;
}
long get_file_size(char *filename){
    FILE *fp = NULL;
    
    if((fp = fopen(filename,"r")) == NULL){
        printf("获取文件失败");
        return -1;
    }
    fseek(fp,0,SEEK_END);
    //获取文件大小
    long file_size = ftell(fp);
    
    fclose(fp);
    return file_size;
}
char  *set_file2buf(char *filename){
    FILE *fp = NULL;
    char *buffer = NULL;
    if((fp = fopen(filename,"r")) == NULL){
        printf("获取文件失败");
        return NULL;
    }
    fseek(fp,0,SEEK_END);
    //获取文件大小
    long file_size = ftell(fp);
    buffer = malloc(file_size);
    memset(buffer,0,file_size);
    rewind(fp);
    fread(buffer,1,file_size,fp);
    fclose(fp);
    return buffer;

}

int main(int argc,char *args[]){
    char *filename = "test.txt";
    int server_fd = server_listen();
    int client_fd = -1;
    struct package_t package = {0};
    //pthread_create();
    while(1){
        client_fd= accept_request(server_fd);
        package.filename = filename;
        package.file_content = set_file2buf(filename);
        package.package_len = 4 + strlen(filename)+1 + get_file_size(filename);
        write(client_fd,&package.package_len,4);
        write(client_fd,package.filename,strlen(filename)+1);
        write(client_fd,package.file_content,get_file_size(filename));
        
        close(client_fd);
    }
    
}
