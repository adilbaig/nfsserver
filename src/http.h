#define MAXLINE_LEN 1024
#define MAX_POST_SIZE MAXLINE_LEN
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"

struct http_url {
    char scheme[MAXLINE_LEN];
    char domain[MAXLINE_LEN];
    unsigned short port;
    char path[MAXLINE_LEN];
    char query_string[MAXLINE_LEN];
};

struct http_headers {
    char method[MAXLINE_LEN]; //TODO: Enums?
    char uri[MAXLINE_LEN];
    char version[MAXLINE_LEN];
    char user_agent[MAXLINE_LEN];
    char host[MAXLINE_LEN];
    char accept[MAXLINE_LEN];
};

struct http_request_data {
    struct http_headers headers;
    struct http_url url;
    char body[MAX_POST_SIZE];
};

struct http_request_data http_parse_request(const int fd);

void http_extract_header(char *key, char *buf, char *copy_to);

void http_print_request_data(struct http_request_data *data);
