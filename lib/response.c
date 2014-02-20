#include <stdio.h>
#include "response.h"

struct response *response_create() {
    struct response *res = (struct response *)malloc(sizeof(struct response));
    return res;
}

void response_destroy(struct response *res) {
    free(res->content);
    free(res);
}

void response_populate(char *path, struct response *res) {
    FILE *fp = fopen(path, "r");
    // Handle file opening errors
    int fd = fileno(fp);
    struct stat st;
    fstat(fd, &st);
    // Handle errors returned by fstat
    res->type = HTML;
    res->length = st.st_size;
    res->content = malloc(st.st_size);
    fread(res->content, st.st_size, 1, fp);
    fclose(fp);
}

const char *get_content_type_str(enum content_type type) {
    switch (type) {
        case HTML:
            return "text/html";
        case JPG:
            return "image/jpg";
        case PNG:
            return "image/png";
        default:
            return "text/custom";
    }
}
