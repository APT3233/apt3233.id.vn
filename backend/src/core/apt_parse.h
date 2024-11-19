#ifndef _PARSE_H_
#define _PARSE_H_

#include <sys/types.h>

typedef struct {
    char method[16];
    char path[256];
    char protocol[16];
    size_t content_length;
    char* body;
} HttpRequest;

extern char *find_start_of_body(char *header);
extern int parse_http_request(int fd, char *request, int bytes_recvd, HttpRequest *http_request);

#endif