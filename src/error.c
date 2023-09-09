#include <stdarg.h>

#include "interpreter.h"

void runtime_error(const char *message, ...) {
    va_list args;
    va_start(args, message);
    fprintf(stderr, "Runtime error: ");
    vfprintf(stderr, message, args);
    exit(1);
}
