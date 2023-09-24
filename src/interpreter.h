#include <json-c/json.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define match(s1, s2) strcmp(s1, s2) == 0
#define alloc(t) malloc(sizeof(t))
#define len(a) sizeof(a) / sizeof(a[0])

typedef struct {
    size_t length;
    void **values;
} array_t;

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

typedef struct {
    const char *kind;
    term_t *condition;
    term_t *then;
    term_t *otherwise;
} if_t;

typedef struct {
    const char *text;
} parameter_t;

typedef struct {
    const char *kind;
    parameter_t *name;
    term_t *value;
    term_t *next;
} let_t;

typedef struct {
    const char *kind;
    const char *text;
} var_t;

typedef struct {
    const char *kind;
    term_t *callee;
    array_t *arguments;
} call_t;

typedef struct {
    const char *key;
    result_t *value;
} variable_t;

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
if_t *make_if_t(term_t *condition, term_t *then, term_t *otherwise);
parameter_t *make_parameter_t(const char *text);
let_t *make_let_t(parameter_t *name, term_t *value, term_t *next);
var_t *make_var_t(const char *text);
call_t *make_call_t(term_t *callee, array_t *arguments);
variable_t *make_variable_t(const char *key, result_t *value);
array_t *make_array_t(size_t length, void **values);
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
void free_if_t(if_t *value);
void free_parameter_t(parameter_t *parameter);
void free_let_t(let_t *let);
void free_var_t(var_t *var);
void free_call_t(call_t *call);
void clean();
void free_variable_t(variable_t *variable);
void free_array_t(array_t *array);

void runtime_error(const char *message, ...);

term_t *parse_expression(json_object *expression);
result_t *eval(term_t *root);

typedef struct result_map_t {
    struct result_map_t *next;
    const char *key;
    result_t *value;
} result_map_t;

typedef struct term_map_t {
    struct term_map_t *next;
    const char *key;
    term_t *value;
} term_map_t;

typedef struct stack_t {
    struct stack_t *next;
    struct stack_t *parent;
    result_map_t *variables;
    term_map_t *functions;
} stack_t;

result_map_t *make_result_map_t(const char *key, result_t *value);
void free_result_map_t(result_map_t *result_map);
term_map_t *make_term_map_t(const char *key, term_t *value);
void free_term_map_t(term_map_t *term_map);
stack_t *make_stack_t();
void free_stack_t(stack_t *stack);
int stack_len(stack_t *stack);
stack_t *stack_copy(stack_t *stack);
term_t *lookup_term(term_map_t *map, const char *key);
result_t *lookup_result(result_map_t *map, const char *key);
stack_t *stack_add(stack_t *stack, stack_t *value);
result_map_t *result_map_add(result_map_t *map, result_map_t *value);
result_map_t *result_map_replace(result_map_t *map, result_map_t *value);
term_map_t *term_map_add(term_map_t *map, term_map_t *value);

void printf_c(const char *fmt, ...);
void set_interpreter_stdout(FILE *stdout);

typedef struct {
    const char *kind;
    array_t *parameters;
    term_t *value;
    stack_t *closure_stack;
} function_t;

function_t *make_function_t(array_t *parameters, term_t *value);
void free_function_t(function_t *function);
