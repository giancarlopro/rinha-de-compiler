#include "interpreter.h"

result_t *make_result_t(void *value, const char *type) {
    result_t *result = malloc(sizeof(result_t));

    result->value = value;
    result->type = type;

    return result;
}

void free_result_t(result_t *result) {
    if (result == NULL) return;
    free(result);
}

void free_term_t(term_t *term) {
    if (term == NULL) return;

    if (match(term->kind, "Print")) {
        free_print_t((print_t *)term);
    } else if (match(term->kind, "Str")) {
        free_str_t((str_t *)term);
    } else if (match(term->kind, "Int")) {
        free_int_t((int_t *)term);
    } else if (match(term->kind, "Bool")) {
        free_bool_t((bool_t *)term);
    } else if (match(term->kind, "Tuple")) {
        free_tuple_t((tuple_t *)term);
    } else if (match(term->kind, "First")) {
        free_first_t((first_t *)term);
    } else if (match(term->kind, "Second")) {
        free_second_t((second_t *)term);
    } else if (match(term->kind, "Binary")) {
        free_binary_t((binary_t *)term);
    } else if (match(term->kind, "If")) {
        free_if_t((if_t *)term);
    } else if (match(term->kind, "Let")) {
        free_let_t((let_t *)term);
    } else if (match(term->kind, "Var")) {
        free_var_t((var_t *)term);
    } else if (match(term->kind, "Function")) {
        free_function_t((function_t *)term);
    } else if (match(term->kind, "Call")) {
        free_call_t((call_t *)term);
    }
}

print_t *make_print_t(term_t *value) {
    print_t *print = malloc(sizeof(print_t));

    print->kind = "Print";
    print->value = value;

    return print;
}

void free_print_t(print_t *print) {
    if (print == NULL) return;

    free(print->value);
    free(print);
}

str_t *make_str_t(const char *value) {
    str_t *string = malloc(sizeof(str_t));

    string->kind = "Str";
    string->value = value;

    return string;
}

void free_str_t(str_t *string) {
    if (string == NULL) return;

    free(string);
}

int_t *make_int_t(int value) {
    int_t *integer = malloc(sizeof(int_t));
    integer->value = malloc(sizeof(int));

    integer->kind = "Int";
    *(integer->value) = value;

    return integer;
}

void free_int_t(int_t *integer) {
    if (integer == NULL) return;

    free(integer->value);
    free(integer);
}

bool_t *make_bool_t(bool value) {
    bool_t *boolean = malloc(sizeof(bool_t));
    boolean->value = malloc(sizeof(bool));

    boolean->kind = "Bool";
    *(boolean->value) = value;

    return boolean;
}

void free_bool_t(bool_t *boolean) {
    if (boolean == NULL) return;

    free(boolean->value);
    free(boolean);
}

tuple_t *make_tuple_t(term_t *first, term_t *second) {
    tuple_t *tuple = malloc(sizeof(tuple_t));

    tuple->kind = "Tuple";
    tuple->first = first;
    tuple->second = second;

    return tuple;
}

void free_tuple_t(tuple_t *tuple) {
    if (tuple == NULL) return;

    free_term_t((term_t *)tuple->first);
    free_term_t((term_t *)tuple->second);
    free(tuple);
}

runtime_tuple_t *make_runtime_tuple_t(void *first, void *second) {
    runtime_tuple_t *tuple = malloc(sizeof(runtime_tuple_t));

    tuple->first = first;
    tuple->second = second;

    return tuple;
}

void free_runtime_tuple_t(runtime_tuple_t *tuple) {
    if (tuple == NULL) return;
    free(tuple);
}

first_t *make_first_t(term_t *value) {
    first_t *first = malloc(sizeof(first_t));

    first->kind = "First";
    first->value = value;

    return first;
}

void free_first_t(first_t *first) {
    if (first == NULL) return;

    free_term_t(first->value);
    free(first);
}

second_t *make_second_t(term_t *value) {
    second_t *second = malloc(sizeof(second_t));

    second->kind = "Second";
    second->value = value;

    return second;
}

void free_second_t(second_t *second) {
    if (second == NULL) return;

    free_term_t(second->value);
    free(second);
}

binary_t *make_binary_t(const char *op, term_t *lhs, term_t *rhs) {
    binary_t *binary = malloc(sizeof(binary_t));

    binary->kind = "Binary";
    binary->op = op;
    binary->lhs = lhs;
    binary->rhs = rhs;

    return binary;
}

void free_binary_t(binary_t *binary) {
    if (binary == NULL) return;

    free_term_t(binary->lhs);
    free_term_t(binary->rhs);
    free(binary);
}

if_t *make_if_t(term_t *condition, term_t *then, term_t *otherwise) {
    if_t *value = malloc(sizeof(if_t));

    value->kind = "If";
    value->then = then;
    value->condition = condition;
    value->otherwise = otherwise;

    return value;
}

void free_if_t(if_t *value) {
    if (value == NULL) return;

    free_term_t(value->condition);
    free_term_t(value->then);
    free_term_t(value->otherwise);
    free(value);
}

parameter_t *make_parameter_t(const char *text) {
    parameter_t *parameter = malloc(sizeof(parameter_t));

    parameter->text = text;

    return parameter;
}

void free_parameter_t(parameter_t *parameter) {
    if (parameter == NULL) return;
    free(parameter);
}

let_t *make_let_t(parameter_t *name, term_t *value, term_t *next) {
    let_t *let = malloc(sizeof(let_t));

    let->kind = "Let";
    let->name = name;
    let->value = value;
    let->next = next;

    return let;
}

void free_let_t(let_t *let) {
    if (let == NULL) return;

    free_parameter_t(let->name);
    free_term_t(let->value);
    free_term_t(let->next);
    free(let);
}

var_t *make_var_t(const char *text) {
    var_t *var = malloc(sizeof(var_t));

    var->kind = "Var";
    var->text = text;

    return var;
}

void free_var_t(var_t *var) {
    if (var == NULL) return;
    free(var);
}

function_t *make_function_t(array_t *parameters, term_t *value) {
    function_t *function = malloc(sizeof(function_t));

    function->kind = "Function";
    function->parameters = parameters;
    function->value = value;

    return function;
}

void free_function_t(function_t *function) {
    if (function == NULL) return;

    free_term_t(function->value);
    free(function);
}

call_t *make_call_t(term_t *callee, array_t *arguments) {
    call_t *call = malloc(sizeof(call_t));

    call->kind = "Call";
    call->callee = callee;
    call->arguments = arguments;

    return call;
}

void free_call_t(call_t *call) {
    if (call == NULL) return;

    free_term_t(call->callee);
    free(call);
}

variable_t *make_variable_t(const char *key, result_t *value) {
    variable_t *variable = malloc(sizeof(variable_t));

    variable->key = key;
    variable->value = value;

    return variable;
}

void free_variable_t(variable_t *variable) {
    if (variable == NULL) return;

    free_result_t(variable->value);
    free(variable);
}

array_t *make_array_t(size_t length, void **values) {
    array_t *array = malloc(sizeof(array_t));

    array->length = length;
    array->values = values;

    return array;
}

void free_array_t(array_t *array) {
    if (array == NULL) return;

    for (size_t i = 0; i < array->length; i++) {
        if (array->values[i] != NULL) free(array->values[i]);
    }

    free(array->values);
    free(array);
}
