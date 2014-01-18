#include "common.h"

void http_extract_header(char *key, char *buf, char *copy_to) {
    size_t len = strlen(key);
    if (strncmp(key, buf, len) == 0) {
        char format[len + 10];
        sprintf(format, "%s: %s", key, "%[^" CRLF "]");
        sscanf(buf, format, copy_to);
        /*sscanf(buf, "User-Agent: %[^\n\r]", header_data.user_agent);*/
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

    return data;
}

#define PRINT_STRUCT_DATA(field) printf(#field ": %s \n", headers->field);
void http_print_request_data(struct http_request_data *data) {

    struct http_headers *headers;
    headers = &data->headers;

    PRINT_STRUCT_DATA(method)
    PRINT_STRUCT_DATA(uri)
    PRINT_STRUCT_DATA(version)
    PRINT_STRUCT_DATA(user_agent)
    PRINT_STRUCT_DATA(host)
    PRINT_STRUCT_DATA(accept)
}
