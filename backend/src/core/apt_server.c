#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>

#include "apt_net.h"
#include "apt_file.h"
#include "apt_mime.h"
#include "apt_cache.h"
#include "apt_hashtable.h"
#include "apt_linkedlist.h"
#include "apt_parse.h"
#include "../utils/apt_time.h"


#define PORT "4444" 

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./www"


void log_info(int type, const char* title, const char* msg)
{
    if(type < 0) 
        printf("[\033[1;36mERROR\033[0m][\033[0;35m%s\033[0m] %s %s\n", get_current_time(), title, msg);
    else if(type == 0) 
        printf("[\033[1;36mWARNING\033[0m][\033[0;35m%s\033[0m] %s %s\n", get_current_time(), title, msg);
    else 
        printf("[\033[1;36mINFO\033[0m][\033[0;35m%s\033[0m] %s %s\n", get_current_time(), title, msg);
    
}


/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */
int send_response(int fd, char *header, char *content_type, const void *body, size_t content_length)
{
    const size_t max_response_size = 262144;
    char response[max_response_size];

    int header_length = snprintf(response, max_response_size,
                                 "%s\r\n"
                                 "Content-Type: %s\r\n"
                                 "Content-Length: %zu\r\n"
                                 "\r\n",
                                 header, content_type, content_length);

    if (header_length < 0) {
        perror("Header creation failed");
        return -1;
    }

    if((size_t)header_length >= max_response_size)
    {
        fprintf(stderr, "Header length exceeds buffer size\n");
        return -1;
    }

    // Ensure we do  not overflow the buffer
    if ((size_t)header_length + content_length >= max_response_size) {
        fprintf(stderr, "Response too large for buffer");
        return -1;
    }

    memcpy(response + header_length, body, content_length);  // Append the body after the header

    // Send it all!
    size_t response_length = (size_t)header_length + content_length;
    ssize_t rv = send(fd, response, response_length, 0);

    if (rv < 0) {
        perror("send");
    }

    return rv;
}

/**
 * Send a /d20 endpoint response
 */
void get_rand(int fd)
{
    log_info(1, "Fetching Path: ", "/rand");
    srand(time(NULL)); 
    long long random_number = (rand() % 99000000) + 1000000;

    char response_body[16];  
    int bytes_written = snprintf(response_body, sizeof(response_body), "%lld", random_number);
    if (bytes_written < 0 || (size_t)bytes_written >= sizeof(response_body)) {
        log_info(-1, "Get rand: ", "Failed to create response body");
        const char *error_body = "Internal Server Error";
        send_response(fd, "HTTP/1.1 500 Internal Server Esrror", "text/plain", error_body, strlen(error_body));
        return;
    }

    char* res_rand;
    const char* title = "Random Number";
    asprintf(&res_rand, "<html><head><title>%s</title></head><body><h1>%s</h1></body></html>", title, response_body);

    ssize_t res = send_response(fd, "HTTP/1.1 200 OK", "text/html", res_rand, strlen(res_rand));
    log_info(1, "Rand: ", response_body);
    log_info(1, "Completed", "\n");

    free(res_rand);

    if(res<0)
    {
        fprintf(stderr, "Failed to send random number\n");
        return;
    }
}

/**
 * Send a 404 response
 */
void resp_404(int fd, Cache *cache)
{
    char *filepath = malloc(2048);  // Cấp phát động
    if (filepath == NULL) {
        perror("malloc failed");
        return; 
    }
    struct file_data *filedata; 
    char *mime_type;

    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    Cache_Entry *entry = cache_get(cache, filepath);
    if(entry != NULL)
    {
        send_response(fd, "HTTP/1.1 404 NOT FOUND", entry->content_type, entry->content, entry->content_length);
        log_info(1, "Completed", "\n");
        return;
    }
    
    filedata = file_load(filepath);
    mime_type = mime_type_get(filepath);
    if (filedata == NULL) {
        char *default_404_body = "<html><body><h1>404 Not Found</h1></body></html>";
        send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, default_404_body, strlen(default_404_body));
        return;
    }

    file_free(filedata);
    free(filepath);
}


int is_path_safe(const char *path) {
    if (strstr(path, "..") != NULL) {
        return 0; // not safe
    }
    return 1; 
}
/**
 * Read and return a file from disk or cache
 */
void get_file(int fd, Cache *cache, char *request_path)
{
    char filepath[4096];
    struct file_data *filedata;
    char *mime_type;
    
    if (request_path == NULL || strcmp(request_path, "/") == 0) {
        request_path = "/index.html";
    }

    if (!is_path_safe(request_path)) {
        log_info(-1, "Unsafe path requested: %s", request_path);
        resp_404(fd, cache);
        return;
    }

    snprintf(filepath, sizeof(filepath), "%s%s", SERVER_ROOT, request_path);
    log_info(1, "Fetching Path: ", filepath);
    Cache_Entry *entry = cache_get(cache, filepath);
    if (entry != NULL) {
        ssize_t res = send_response(fd, "HTTP/1.1 200 OK", entry->content_type, entry->content, entry->content_length);
        log_info(1, "Completed","\n");
        if (res < 0) 
            log_info(-1, "Failed to send cached response for: %s", filepath);
        
        return;
    }

    filedata = file_load(filepath);
    if (filedata == NULL) {
        log_info(-1, "File not found: ", filepath);
        printf("\n");
        resp_404(fd, cache);
        return;
    }

    mime_type = mime_type_get(filepath);
    cache_put(cache, filepath, mime_type, filedata->data, filedata->size);
    ssize_t res = send_response(fd, "HTTP/1.1 200 OK", mime_type, filedata->data, filedata->size);
    if (res < 0) 
        log_info(-1, "Failed to send response for file: ", filepath);
    else 
        log_info(1, "Completed","\n");
    
    file_free(filedata);
}

/**
 * handle GET
 */
void handle_get(int fd, Cache* cache, char* path)
{
    if(strcmp(path, "/rand") == 0)
        get_rand(fd);
    else
        get_file(fd, cache, path);
}

/**
 * Handle HTTP request and send response
 */
void handle_http_request(int fd, Cache *cache)
{
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];
    HttpRequest http_request;
    memset(&http_request, 0, sizeof(HttpRequest));

    // Read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0) {
        perror("recv");
        return;
    }
    request[bytes_recvd] = '\0'; 
    
    log_info(1, "Parsing...", "");
    if (parse_http_request(fd, request, bytes_recvd, &http_request) != 0)
    {
        send_response(fd, "HTTP/1.1 400 BAD REQUEST", "text/plain", "Bad Request", 11);
        return;
    }

    if (strcmp(http_request.method, "GET") == 0) 
        handle_get(fd, cache, http_request.path);
    
    else if (strcmp(http_request.method, "POST") == 0) 
        send_response(fd, "HTTP/1.1 405 METHOD NOT ALLOWED", "text/plain", "Method Not Allowed", 15);
    else 
        send_response(fd, "HTTP/1.1 501 NOT IMPLEMENTED", "text/plain", "Not Implemented", 14);
    
}

/**
 * Main
 */
int main(void)
{
    int newfd;  // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];
    size_t req_total = 0;

    Cache *cache = cache_create(30, 0);

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0) {
        fprintf(stderr, "webserver: fatal error getting listening socket\n");
        exit(1);
    }

    log_info(1, "Waiting for connections on port ", "4444\n");

    // This is the main loop that accepts incoming connections and
    // responds to the request. The main parent process
    // then goes back to waiting for new connections.
    
    while(1) {
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
        newfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);

        req_total++;
        char req_count[32];
        snprintf(req_count, sizeof(req_total), "%zu" ,req_total);
        log_info(1, "Requests: ", req_count);
        log_info(1, "Server got connection from", s);
        
        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.

        handle_http_request(newfd, cache);

        close(newfd);
    }

    // Unreachable code

    return 0;
}
