#include "apt_parse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *find_start_of_body(char *header)
{
    char *body_start = strstr(header, "\r\n\r\n"); // Look for the end of the header
    if (body_start == NULL) {
        return NULL; // No body found
    }
    return body_start + 4; // Move past the "\r\n\r\n"
}

int parse_http_request(int fd, char *request, int bytes_recvd, HttpRequest *http_request)
{

    if (sscanf(request, "%15s %255s %15s", http_request->method, http_request->path, http_request->protocol) != 3)
    {
        return -1; 
    }


    char *content_length_str = strstr(request, "Content-Length:");
    if (content_length_str)
        sscanf(content_length_str, "Content-Length: %zu", &http_request->content_length);
    else
        http_request->content_length = 0;
    


    char *body_start = find_start_of_body(request);
    if (body_start && http_request->content_length > 0)
    {
        int header_length = body_start - request;
        int body_received = bytes_recvd - header_length;


        if ((size_t)body_received >= http_request->content_length)
        {
            http_request->body = strndup(body_start, http_request->content_length);
            if (http_request->body == NULL)
                return -1;
            
        }
        else
        {
            int remaining = http_request->content_length - body_received;
            http_request->body = malloc(http_request->content_length + 1);
            if (http_request->body == NULL)
                return -1;
            
            memcpy(http_request->body, body_start, body_received);
            ssize_t additional_recv = recv(fd, http_request->body + body_received, remaining, 0);
            if (additional_recv < 0)
            {
                free(http_request->body);
                return -1;
            }
            http_request->body[body_received + additional_recv] = '\0';
        }
    }

    else
        http_request->body = NULL;
    

    return 0;
}