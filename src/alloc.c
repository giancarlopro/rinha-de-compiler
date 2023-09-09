#include "interpreter.h"

result_t *make_result_t(void *value, const char *type) {
    result_t *result = malloc(sizeof(result_t));

    result->value = value;
    result->type = type;

    return result;
}

void free_result_t(result_t *result) { free(result); }

void free_term_t(term_t *term) {
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
    }
}

print_t *make_print_t(term_t *value) {
    print_t *print = malloc(sizeof(print_t));

    print->kind = "Print";
    print->value = value;

    return print;
}

void free_print_t(print_t *print) {
    free(print->value);
    free(print);
}

str_t *make_str_t(const char *value) {
    str_t *string = malloc(sizeof(str_t));

    string->kind = "Str";
    string->value = value;

    return string;
}

void free_str_t(str_t *string) { free(string); }

int_t *make_int_t(int value) {
    int_t *integer = malloc(sizeof(int_t));
    integer->value = malloc(sizeof(int));

    integer->kind = "Int";
    *(integer->value) = value;

    return integer;
}

void free_int_t(int_t *integer) {
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

void free_runtime_tuple_t(runtime_tuple_t *tuple) { free(tuple); }

first_t *make_first_t(term_t *value) {
    first_t *first = malloc(sizeof(first_t));

    first->kind = "First";
    first->value = value;

    return first;
}

void free_first_t(first_t *first) {
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
    free_term_t(value->condition);
    free_term_t(value->then);
    free_term_t(value->otherwise);
    free(value);
}
