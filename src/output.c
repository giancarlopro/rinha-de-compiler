#include "interpreter.h"

FILE *program_stdout = NULL;

void printf_c(const char *fmt, ...) {
    if (program_stdout == NULL) {
        program_stdout = stdout;
    }

    va_list args;
    va_start(args, fmt);

    vfprintf(program_stdout, fmt, args);

    va_end(args);
}

void set_interpreter_stdout(FILE *stdout) { program_stdout = stdout; }
