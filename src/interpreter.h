#include <json-c/json.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define match(s1, s2) strcmp(s1, s2) == 0
#define alloc(t) malloc(sizeof(t))

typedef struct {
    void *value;
    const char *type;
} result_t;

typedef struct {
    const char *kind;
    void *value;
} term_t;

typedef struct {
    const char *kind;
    term_t *value;
} print_t;

typedef struct {
    const char *kind;
    const char *value;
} str_t;

typedef struct {
    const char *kind;
    bool *value;
} bool_t;

typedef struct {
    const char *kind;
    int *value;
} int_t;

typedef struct {
    const char *kind;
    term_t *first;
    term_t *second;
} tuple_t;

typedef struct {
    void *first;
    void *second;
} runtime_tuple_t;

typedef struct {
    const char *kind;
    term_t *value;
} first_t;

typedef struct {
    const char *kind;
    term_t *value;
} second_t;

typedef struct {
    const char *kind;
    const char *op;
    term_t *lhs;
    term_t *rhs;
} binary_t;

result_t *make_result_t(void *value, const char *type);
print_t *make_print_t(term_t *value);
str_t *make_str_t(const char *value);
int_t *make_int_t(int value);
bool_t *make_bool_t(bool value);
tuple_t *make_tuple_t(term_t *first, term_t *second);
runtime_tuple_t *make_runtime_tuple_t(void *first, void *second);
first_t *make_first_t(term_t *value);
second_t *make_second_t(term_t *value);
binary_t *make_binary_t(const char *op, term_t *lhs, term_t *rhs);
void free_term_t(term_t *term);
void free_result_t(result_t *result);
void free_print_t(print_t *print);
void free_str_t(str_t *string);
void free_int_t(int_t *integer);
void free_bool_t(bool_t *boolean);
void free_tuple_t(tuple_t *tuple);
void free_runtime_tuple_t(runtime_tuple_t *tuple);
void free_first_t(first_t *first);
void free_second_t(second_t *second);
void free_binary_t(binary_t *binary);

void runtime_error(const char *message, ...);

term_t *parse_expression(json_object *expression);
result_t *eval(term_t *root);
