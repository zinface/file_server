#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc,char *args[]){
    FILE *fp = NULL;
    
    struct sockaddr_in client = {0};
    client.sin_family =  AF_INET;
    client.sin_port = htons(8082);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    int client_fd = socket(AF_INET,SOCK_STREAM,0);
    uint32_t package_len = 0;
    char src_filename[100] = {0};
    char * file_content = malloc(1000);
    
    connect(client_fd,(struct sockaddr *)&client,sizeof(client));
    
    read(client_fd,&package_len,4);
    read(client_fd,src_filename,100);
    read(client_fd,file_content,package_len-4);
    fp = fopen(src_filename,"w+");
    fwrite(file_content,1,package_len-4-strlen(src_filename)-1,fp);
    close(client_fd);
    free(file_content);
    return 0;
    
}