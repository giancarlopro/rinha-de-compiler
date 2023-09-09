#include <stdarg.h>

#include "interpreter.h"

void runtime_error(const char *message, ...) {
    fprintf(stderr, "Runtime error: %s\n", message);
    exit(1);
}
