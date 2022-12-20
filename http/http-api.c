#include "http/http-api.h"

#define BUF_SIZE 1024

int http_init(struct http *dst, char *host, in_port_t port)
{
    strncpy(dst->hostname, host, 64);
    dst->port = port;
}

static int _http_response(int fd, char **dst_body, unsigned int *buf_size)
{
    int ret_code;
    int bytes;
    char buffer[BUF_SIZE];
    FILE *fp = fdopen(fd, "r");
    char http_header[] = "HTTP/";
    size_t line_length = 0;
    char *lineptr = NULL;
    char *ptr;

    // Fetching header
    while(getdelim(&lineptr, &line_length, '\n', fp) != -1)
    {
        // printf("%d - %s", line_length, lineptr);
        // printf("%s", lineptr);
        if(ptr = strstr(lineptr, http_header))
        {
            ret_code = atoi(ptr + sizeof(http_header) + 2);
            // printf("RETCODE %d\n", ret_code);
        }
        
        if(strcmp(lineptr, "\r\n") == 0)
        {
            break;
        }
    }
    free(lineptr);
    
    
    if(*dst_body == NULL)
    {
        *buf_size = 256;
        *dst_body = malloc(*buf_size);
    }
    else if(*buf_size <= 0)
    {
        *buf_size = 256;
        *dst_body = realloc(*dst_body, *buf_size);
    }

    bytes = 0;
    int n_read;
    bzero(*dst_body, *buf_size);
    // (n_read = read(fd, buffer, BUF_SIZE - 1)) != 0
    while( (n_read = fread(buffer, 1, BUF_SIZE - 1, fp)) != 0 ){
        // fprintf(stderr, "%d - %s", n_read, buffer);
        bytes += n_read;
        
        while(bytes >= *buf_size)
        {
            *buf_size *= 2;
            // printf("JALAP %d %x\n", *buf_size, *dst_body);
            *dst_body = realloc(*dst_body, *buf_size);
            // printf("PIDOR\n");
        }

        strncat(*dst_body, buffer, n_read);

        bzero(buffer, BUF_SIZE);
    }

    *dst_body = realloc(*dst_body, bytes + 1);
    *buf_size = bytes + 1;
    (*dst_body)[bytes] = '\0';
    fclose(fp);
    shutdown(fd, SHUT_RDWR);

    close(fd);
    return ret_code;
}

int http_get(struct http *dst, char *route, char **dst_response, unsigned int *dst_response_size)
{
    int fd = user_socket_connect(dst->hostname, dst->port);
    char buffer[BUF_SIZE];
    int pos = 0;
    int bytes;
    int ret_code;

    if(fd == -1)
    {
        fprintf(stderr, "Socket falure\n");
    }

    bzero(buffer, BUF_SIZE);
    pos += snprintf(buffer + pos, BUF_SIZE, "GET %s HTTP/1.0\r\n", route);
    pos += snprintf(buffer + pos, BUF_SIZE - pos, "\r\n");
    
    bytes = write(fd, buffer, pos);
    if(bytes != pos)
    {
        fprintf(stderr, "Write func failure");
    }
    // bzero(dst_body, buf_size);
    // printf("REQUEST\n%s\n", buffer);

    ret_code = _http_response(fd, dst_response, dst_response_size);

    return ret_code;
}


int http_post(struct http *dst, char *route, char *post_body, unsigned int post_body_size, char **dst_response, unsigned int *dst_response_size)
{
    int fd = user_socket_connect(dst->hostname, dst->port);
    char buffer[BUF_SIZE];
    int pos = 0;
    int bytes;
    int ret_code;

    if(fd == -1)
    {
        fprintf(stderr, "Socket falure\n");
    }

    bzero(buffer, BUF_SIZE);
    pos += snprintf(buffer + pos, BUF_SIZE, "POST %s HTTP/1.0\r\n", route);
    pos += snprintf(buffer + pos, BUF_SIZE - pos, "Content-Type: application/json\r\n");
    pos += snprintf(buffer + pos, BUF_SIZE - pos, "Content-Length: %d\r\n", post_body_size-1);
    pos += snprintf(buffer + pos, BUF_SIZE - pos, "\r\n");
    // printf("POST HEADER\n%s\n", buffer);
    bytes = write(fd, buffer, pos);
    if(bytes != pos)
    {
        fprintf(stderr, "Write func failure");
    }
    bytes = write(fd, post_body, post_body_size);
    if(bytes != post_body_size)
    {
        fprintf(stderr, "Write func failure");
    }
    

    ret_code = _http_response(fd, dst_response, dst_response_size);
    
    return ret_code;
}