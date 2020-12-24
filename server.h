#ifndef __SERVER_H_
#define __SERVER_H_

#define LISTEN_PORT 8082
//#define LISTEN_ADDR INADDR_ANY; //绑定所有ip地址
#define LISTEN_ADDR "127.0.0.1"
struct package_t
{
    uint32_t package_len;
    char *filename;
    char *file_content;
};

#endif // __SERVER_H_