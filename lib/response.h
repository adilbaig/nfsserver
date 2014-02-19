#include <sys/stat.h>
#include "../lib/common.h"

enum content_type {
    HTML,
    TEXT,
    JPG,
    PNG,
    APPLICATION
};

struct response {
    enum content_type type;
    size_t length;
    void *content;
};

struct response *response_create();
void response_destroy(struct response *res);
void response_populate(char *path, struct response *res);
const char *get_content_type_str(enum content_type type);
