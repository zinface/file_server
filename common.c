#include "common.h"
#include <string.h>
#include <stdlib.h>

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