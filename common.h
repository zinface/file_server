#ifndef __COMMON_H_
#define __COMMON_H_

#include <sys/socket.h>
#include <arpa/inet.h>

#define DATA_PART 4096

struct package_t
{
    uint32_t package_len;
    uint32_t _start;
    char filename[256];
    char *file_content;
}; // plen/filename/conten_len

void buildServer(struct sockaddr_in* servaddr, sa_family_t family, char* address , in_port_t port);
char *createBufferSize(long _msize);

#endif // __COMMON_H_