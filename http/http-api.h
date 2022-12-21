#include "http/socket_linux.h"

struct http
{
    char hostname[64];
    in_port_t port;
};

int http_init(struct http *dst, char *host, in_port_t port);
// If you want empty body in GET HTTP Request, set get_body = NULL
int http_get(struct http *dst, char *route, char *get_body, 
            unsigned int get_body_size, char **response_body, unsigned int *response_body_size);
int http_post(struct http *dst, char *route, char *post_body, \
            unsigned int post_body_size, char **response_body, unsigned int *response_body_size);

int http_put(struct http *dst, char *route, char *post_body, \
            unsigned int post_body_size, char **response_body, unsigned int *response_body_size);

int http_delete(struct http *dst, char *route, char **response_body, unsigned int *response_body_size);

