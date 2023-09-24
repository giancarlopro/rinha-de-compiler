#include <stdio.h>

#include "interpreter.h"

int main(int argc, char **argv) {
    char *filename = argv[1];

    json_object *root = NULL;

    if (match(filename, "-")) {
        root = json_object_from_fd(0);
    } else {
        root = json_object_from_file(filename);
    }

    if (!root) return 1;

    json_object *expression = json_object_object_get(root, "expression");

    term_t *term = parse_expression(expression);

    eval(term);

    free_term_t(term);
    json_object_put(root);
    return 0;
}
