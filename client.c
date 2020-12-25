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

int main(int argc,char *args[]){
    struct sockaddr_in client = {0};
    struct package_t package = {0};
    int client_fd;
    FILE *fp = NULL;
    
    client_fd = Socket(AF_INET, SOCK_STREAM, 0);
    buildServer(&client, AF_INET, "127.0.0.1", 8082);

    connect(client_fd,(struct sockaddr *)&client,sizeof(client));
    
    read(client_fd, &package.package_len, 4);
    read(client_fd, package.filename, sizeof(package.filename));
    
    package.file_content = createBufferSize(package.package_len);
    read(client_fd, package.file_content, package.package_len);
    
    fp = fopen(package.filename, "w+");
    fwrite(package.file_content, 1, package.package_len, fp);
    close(client_fd);
    free(package.file_content);

    printf("file is save: %s\n", package.filename);
    return 0;
}