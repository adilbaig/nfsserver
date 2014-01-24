#include "common.h"

/*Static function prototypes*/
static void http_extract_header(char *key, char *buf, char *copy_to);
static void http_parse_url(char *host, char *uri, struct http_url *url);


static void http_extract_header(char *key, char *buf, char *copy_to) {
    size_t len = strlen(key);
    if (strncmp(key, buf, len) == 0) {
        char format[len + 10];
        sprintf(format, "%s: %s", key, "%[^" CRLF "]");
        sscanf(buf, format, copy_to);
    }
}

struct http_request_data http_parse_request(const int fd) {

    struct http_request_data data;
    char buf[MAXLINE_LEN];

    struct http_headers *headers;
    headers = &data.headers;

    if (getline_from_fd(fd, buf, MAXLINE_LEN)) {
        sscanf(buf, "%s %s %s", headers->method, headers->uri,
                headers->version);
    } else {
        perror("The request does not seem to be a valid http request");
    }

    while(getline_from_fd(fd, buf, MAXLINE_LEN)) {

        if (strcmp(CRLF, buf) == 0)
            break;

        http_extract_header("User-Agent", buf, headers->user_agent);
        http_extract_header("Host", buf, headers->host);
        http_extract_header("Accept", buf, headers->accept);
    }

    http_parse_url(headers->host, headers->uri, &(data.url));

    return data;
}

static void http_parse_url(char *host, char *uri, struct http_url *url) {

    sscanf(host, "%[^:]:%d", url->host, &url->port);
    sscanf(uri, "%[^?]%s", url->path, url->query_string);
    strcpy(url->scheme, HTTP);
}

#define PRINT_STRUCT(obj, field) printf(#field ": %s \n", obj->field);
void http_print_request_data(struct http_request_data *data) {

    struct http_headers *headers;
    struct http_url *url;
    headers = &data->headers;
    url = &data->url;

    PRINT_STRUCT(headers, method)
    PRINT_STRUCT(headers, uri)
    PRINT_STRUCT(headers, version)
    PRINT_STRUCT(headers, user_agent)
    PRINT_STRUCT(headers, host)
    PRINT_STRUCT(headers, accept)
    PRINT_STRUCT(url, scheme)
    PRINT_STRUCT(url, host)
    printf("port: %d \n", url->port);
    PRINT_STRUCT(url, path)
    PRINT_STRUCT(url, query_string)
}
