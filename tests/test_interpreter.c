#include <json-c/json.h>
#include <stdio.h>
#include <string.h>

#include "../src/interpreter.h"
#include "minunit.h"

int tests_run = 0;
const char* STR_JSON = "{\"kind\": \"Str\", \"value\": \"data\"}";
const char* PRINT_JSON =
    "{\"kind\": \"Print\", \"value\": {\"kind\": \"Str\", \"value\": "
    "\"data\"}}";

static char* test_parse_str_expression() {
    json_object* root = json_tokener_parse(STR_JSON);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Str\"", strcmp(term->kind, "Str") == 0);
    mu_assert("error, value != \"data\"",
              strcmp(((char*)term->value), "data") == 0);
}

static char* test_parse_print_expression() {
    json_object* root = json_tokener_parse(PRINT_JSON);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Print\"",
              strcmp(term->kind, "Print") == 0);

    str_t* str = term->value;

    mu_assert("error, str->kind != \"Str\"", strcmp(str->kind, "Str") == 0);
    mu_assert("error, str->value != \"data\"", strcmp(str->value, "data") == 0);
}

static char* test_eval_str() {
    str_t* str = malloc(sizeof(str_t));

    str->kind = "Str";
    str->value = "data";

    result_t* res = eval((term_t*)str);

    mu_assert("error, value != \"data\"", match(res->value, "data"));

    free(str);
}

static char* test_eval_int() {
    int_t* integer = malloc(sizeof(int_t));
    integer->value = malloc(sizeof(int));

    integer->kind = "Int";
    *(integer->value) = 1;

    result_t* res = eval((term_t*)integer);

    mu_assert("error, value != 1", (*(int*)res->value) == 1);

    free(integer->value);
    free(integer);
}

static char* test_eval_bool() {
    bool_t* boolean = malloc(sizeof(bool_t));
    boolean->value = malloc(sizeof(bool));

    boolean->kind = "Bool";
    *(boolean->value) = true;

    result_t* res = eval((term_t*)boolean);

    mu_assert("error, value != true", (*(bool*)res->value) == true);

    free(boolean->value);
    free(boolean);
}

static char* test_eval_tuple() {
    tuple_t* tuple =
        make_tuple_t((term_t*)make_int_t(1), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)tuple);

    runtime_tuple_t* rtuple = (runtime_tuple_t*)res->value;

    mu_assert("tuple error, value[0] != 1", (*(int*)rtuple->first == 1));
    mu_assert("tuple error, value[1] != 2", (*(int*)rtuple->second == 2));

    free_tuple_t(tuple);
}

static char* test_eval_first() {
    tuple_t* tuple =
        make_tuple_t((term_t*)make_int_t(10), (term_t*)make_int_t(20));

    first_t* first = make_first_t((term_t*)tuple);

    result_t* res = eval((term_t*)first);

    mu_assert("First error, value != 10", (*(int*)res->value) == 10);

    free_first_t(first);
}

static char* test_eval_second() {
    tuple_t* tuple =
        make_tuple_t((term_t*)make_int_t(10), (term_t*)make_int_t(20));

    second_t* second = make_second_t((term_t*)tuple);

    result_t* res = eval((term_t*)second);

    mu_assert("Second error, value != 20", (*(int*)res->value) == 20);

    free_second_t(second);
}

static char* test_eval_binary_add() {
    binary_t* binary =
        make_binary_t("Add", (term_t*)make_int_t(1), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("error, value != 3", (*(int*)res->value) == 3);

    free_result_t(res);
    free_binary_t(binary);

    binary = make_binary_t("Add", (term_t*)make_str_t("hello"),
                           (term_t*)make_str_t("world"));

    res = eval((term_t*)binary);

    mu_assert("error, value != \"helloworld\"",
              match(res->value, "helloworld"));

    free_result_t(res);
    free_binary_t(binary);

    binary =
        make_binary_t("Add", (term_t*)make_int_t(2), (term_t*)make_str_t("b"));

    res = eval((term_t*)binary);

    mu_assert("error, value != \"2b\"", match(res->value, "2b"));

    return 0;
}

static char* test_eval_sub() {
    binary_t* binary =
        make_binary_t("Sub", (term_t*)make_int_t(1), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("sub error, value != -1", (*(int*)res->value) == -1);

    return 0;
}

static char* test_eval_mul() {
    binary_t* binary =
        make_binary_t("Mul", (term_t*)make_int_t(3), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("mul error, value != 6", (*(int*)res->value) == 6);

    return 0;
}

static char* test_eval_div() {
    binary_t* binary =
        make_binary_t("Div", (term_t*)make_int_t(3), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("div error, value != 1", (*(int*)res->value) == 1);

    return 0;
}

static char* test_eval_rem() {
    binary_t* binary =
        make_binary_t("Rem", (term_t*)make_int_t(4), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("rem error, value != 0", (*(int*)res->value) == 0);

    return 0;
}

static char* test_eval_lt() {
    binary_t* binary =
        make_binary_t("Lt", (term_t*)make_int_t(1), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("lt error, value != true", (*(int*)res->value) == true);

    free_result_t(res);
    free_binary_t(binary);

    binary =
        make_binary_t("Lt", (term_t*)make_int_t(2), (term_t*)make_int_t(1));

    res = eval((term_t*)binary);

    mu_assert("lt error, value != false", (*(int*)res->value) == false);

    return 0;
}

static char* test_eval_lte() {
    binary_t* binary =
        make_binary_t("Lte", (term_t*)make_int_t(1), (term_t*)make_int_t(1));

    result_t* res = eval((term_t*)binary);

    mu_assert("lte error, value != true", (*(int*)res->value) == true);

    binary =
        make_binary_t("Lte", (term_t*)make_int_t(2), (term_t*)make_int_t(4));

    res = eval((term_t*)binary);

    mu_assert("lte error, value != true", (*(int*)res->value) == true);

    binary =
        make_binary_t("Lte", (term_t*)make_int_t(4), (term_t*)make_int_t(2));

    res = eval((term_t*)binary);

    mu_assert("lte error, value != false", (*(int*)res->value) == false);

    return 0;
}

static char* test_eval_gte() {
    binary_t* binary =
        make_binary_t("Gte", (term_t*)make_int_t(1), (term_t*)make_int_t(1));

    result_t* res = eval((term_t*)binary);

    mu_assert("gte error, value != true", (*(int*)res->value) == true);

    binary =
        make_binary_t("Gte", (term_t*)make_int_t(2), (term_t*)make_int_t(4));

    res = eval((term_t*)binary);

    mu_assert("gte error, value != false", (*(int*)res->value) == false);

    binary =
        make_binary_t("Gte", (term_t*)make_int_t(4), (term_t*)make_int_t(2));

    res = eval((term_t*)binary);

    mu_assert("gte error, value != true", (*(int*)res->value) == true);

    return 0;
}

static char* test_eval_gt() {
    binary_t* binary =
        make_binary_t("Gt", (term_t*)make_int_t(1), (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)binary);

    mu_assert("gt error, value != false", (*(int*)res->value) == false);

    free_result_t(res);
    free_binary_t(binary);

    binary =
        make_binary_t("Gt", (term_t*)make_int_t(2), (term_t*)make_int_t(1));

    res = eval((term_t*)binary);

    mu_assert("gt error, value != true", (*(int*)res->value) == true);

    return 0;
}

static char* test_eval_eq() {
    binary_t* binary =
        make_binary_t("Eq", (term_t*)make_int_t(4), (term_t*)make_int_t(4));

    result_t* res = eval((term_t*)binary);

    mu_assert("eq error, value != true", (*(int*)res->value) == true);

    binary = make_binary_t("Eq", (term_t*)make_str_t("hello"),
                           (term_t*)make_str_t("hello"));

    res = eval((term_t*)binary);

    mu_assert("eq error, value != true", (*(int*)res->value) == true);

    binary =
        make_binary_t("Eq", (term_t*)make_int_t(1), (term_t*)make_int_t(2));

    res = eval((term_t*)binary);

    mu_assert("eq error, value != false", (*(int*)res->value) == false);

    return 0;
}

static char* test_eval_neq() {
    binary_t* binary =
        make_binary_t("Neq", (term_t*)make_int_t(4), (term_t*)make_int_t(4));

    result_t* res = eval((term_t*)binary);

    mu_assert("Neq error, value != false", (*(int*)res->value) == false);

    binary = make_binary_t("Neq", (term_t*)make_str_t("hello"),
                           (term_t*)make_str_t("hello"));

    res = eval((term_t*)binary);

    mu_assert("Neq error, value != false", (*(int*)res->value) == false);

    binary =
        make_binary_t("Neq", (term_t*)make_int_t(1), (term_t*)make_int_t(2));

    res = eval((term_t*)binary);

    mu_assert("Neq error, value != true", (*(int*)res->value) == true);

    return 0;
}

static char* test_eval_and() {
    binary_t* binary = make_binary_t("And", (term_t*)make_bool_t(true),
                                     (term_t*)make_bool_t(false));

    result_t* res = eval((term_t*)binary);

    mu_assert("and error, value != false", (*(int*)res->value) == false);

    binary = make_binary_t("And", (term_t*)make_bool_t(true),
                           (term_t*)make_bool_t(true));

    res = eval((term_t*)binary);

    mu_assert("and error, value != true", (*(int*)res->value) == true);

    return 0;
}

static char* test_eval_or() {
    binary_t* binary = make_binary_t("Or", (term_t*)make_bool_t(true),
                                     (term_t*)make_bool_t(false));

    result_t* res = eval((term_t*)binary);

    mu_assert("Or error, value != true", (*(int*)res->value) == true);

    binary = make_binary_t("Or", (term_t*)make_bool_t(true),
                           (term_t*)make_bool_t(true));

    res = eval((term_t*)binary);

    mu_assert("Or error, value != true", (*(int*)res->value) == true);

    binary = make_binary_t("Or", (term_t*)make_bool_t(false),
                           (term_t*)make_bool_t(false));

    res = eval((term_t*)binary);

    mu_assert("Or error, value != false", (*(int*)res->value) == false);

    return 0;
}

static char* test_eval_if() {
    if_t* if_ = make_if_t((term_t*)make_bool_t(true), (term_t*)make_int_t(1),
                          (term_t*)make_int_t(2));

    result_t* res = eval((term_t*)if_);

    mu_assert("If error, value != 1", (*(int*)res->value) == 1);

    if_ = make_if_t((term_t*)make_bool_t(false), (term_t*)make_int_t(1),
                    (term_t*)make_int_t(2));

    res = eval((term_t*)if_);

    mu_assert("If error, value != 2", (*(int*)res->value) == 2);

    return 0;
}

static char* all_tests() {
    mu_run_test(test_parse_str_expression);
    mu_run_test(test_parse_print_expression);
    mu_run_test(test_eval_str);
    mu_run_test(test_eval_int);
    mu_run_test(test_eval_bool);
    mu_run_test(test_eval_tuple);
    mu_run_test(test_eval_first);
    mu_run_test(test_eval_second);
    mu_run_test(test_eval_binary_add);
    mu_run_test(test_eval_sub);
    mu_run_test(test_eval_mul);
    mu_run_test(test_eval_div);
    mu_run_test(test_eval_rem);
    mu_run_test(test_eval_lt);
    mu_run_test(test_eval_gt);
    mu_run_test(test_eval_lte);
    mu_run_test(test_eval_gte);
    mu_run_test(test_eval_eq);
    mu_run_test(test_eval_neq);
    mu_run_test(test_eval_and);
    mu_run_test(test_eval_or);
    mu_run_test(test_eval_if);
    return 0;
}

int main(int argc, char** argv) {
    char* result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}