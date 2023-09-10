#include "../src/interpreter.h"
#include "minunit.h"

int tests_run = 0;
const char* STR_JSON = "{\"kind\": \"Str\", \"value\": \"data\"}";
const char* PRINT_JSON =
    "{\"kind\": \"Print\", \"value\": {\"kind\": \"Str\", \"value\": "
    "\"data\"}}";

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

static char* test_eval_let() {
    let_t* let = make_let_t(make_parameter_t("x"), (term_t*)make_int_t(1),
                            (term_t*)make_int_t(1));

    result_t* res = eval((term_t*)let);

    mu_assert("Let error, type == Void", !match(res->type, "Void"));
    mu_assert("Let error, value != 1", (*(int*)res->value) == 1);

    return 0;
}

static char* test_eval_var() {
    let_t* let = make_let_t(make_parameter_t("x"), (term_t*)make_int_t(28),
                            (term_t*)make_var_t("x"));

    result_t* res = eval((term_t*)let);

    mu_assert("test_eval_var error, type == Void", !match(res->type, "Void"));
    mu_assert("test_eval_var error, value != 28", (*(int*)res->value) == 28);

    return 0;
}

static char* test_eval_function_no_params() {
    let_t* let =
        make_let_t(make_parameter_t("f"),
                   (term_t*)make_function_t(NULL, (term_t*)make_int_t(33)),
                   (term_t*)make_call_t((term_t*)make_var_t("f"), NULL));

    result_t* res = eval((term_t*)let);

    mu_assert("test_eval_function_no_params error, type == Void",
              !match(res->type, "Void"));
    mu_assert("test_eval_function_no_params error, value != 33",
              (*(int*)res->value) == 33);

    return 0;
}

static char* test_eval_function() {
    parameter_t** parameters = malloc(sizeof(parameter_t*) * 1);
    parameters[0] = make_parameter_t("n");

    var_t* ret_var = make_var_t("n");

    term_t** arguments = malloc(sizeof(term_t*) * 1);
    arguments[0] = (term_t*)make_int_t(13);

    let_t* let =
        make_let_t(make_parameter_t("f"),
                   (term_t*)make_function_t(make_array_t(1, (void**)parameters),
                                            (term_t*)ret_var),
                   (term_t*)make_call_t((term_t*)make_var_t("f"),
                                        make_array_t(1, (void**)arguments)));

    result_t* res = eval((term_t*)let);

    mu_assert("test_eval_function error, type == Void",
              !match(res->type, "Void"));
    mu_assert("test_eval_function error, value != 13",
              (*(int*)res->value) == 13);

    return 0;
}

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

static char* test_parse_int() {
    const char* data = "{\"kind\": \"Int\", \"value\": 1}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Int\"", match(term->kind, "Int"));
    mu_assert("error, term->value != 1", (*(int*)term->value) == 1);

    return 0;
}

static char* test_parse_bool() {
    const char* data = "{\"kind\": \"Bool\", \"value\": true}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Bool\"", match(term->kind, "Bool"));
    mu_assert("error, term->value != true", (*(bool*)term->value) == true);

    return 0;
}

static char* test_parse_tuple() {
    const char* data =
        "{\"kind\": \"Tuple\", \"first\": {\"kind\": \"Int\", "
        "\"value\": 1}, \"second\": {\"kind\": \"Int\", "
        "\"value\": 2}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Tuple\"", match(term->kind, "Tuple"));

    tuple_t* tuple = (tuple_t*)term;

    mu_assert("error, tuple->first->kind != \"Int\"",
              match(tuple->first->kind, "Int"));
    mu_assert("error, tuple->first->value != 1",
              (*(int*)tuple->first->value) == 1);

    mu_assert("error, tuple->second->kind != \"Int\"",
              match(tuple->second->kind, "Int"));
    mu_assert("error, tuple->second->value != 2",
              (*(int*)tuple->second->value) == 2);

    return 0;
}

static char* test_parse_first() {
    const char* data =
        "{\"kind\": \"First\", \"value\": {\"kind\": \"Tuple\", \"first\": "
        "{\"kind\": \"Int\", \"value\": 1}, \"second\": {\"kind\": \"Int\", "
        "\"value\": 2}}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"First\"", match(term->kind, "First"));

    first_t* first = (first_t*)term;

    mu_assert("error, first->value->kind != \"Tuple\"",
              match(first->value->kind, "Tuple"));

    tuple_t* tuple = (tuple_t*)first->value;

    mu_assert("error, tuple->first->kind != \"Int\"",
              match(tuple->first->kind, "Int"));
    mu_assert("error, tuple->first->value != 1",
              (*(int*)tuple->first->value) == 1);

    mu_assert("error, tuple->second->kind != \"Int\"",
              match(tuple->second->kind, "Int"));
    mu_assert("error, tuple->second->value != 2",
              (*(int*)tuple->second->value) == 2);

    return 0;
}

static char* test_parse_second() {
    const char* data =
        "{\"kind\": \"Second\", \"value\": {\"kind\": \"Tuple\", \"first\": "
        "{\"kind\": \"Int\", \"value\": 1}, \"second\": {\"kind\": \"Int\", "
        "\"value\": 2}}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Second\"", match(term->kind, "Second"));

    second_t* second = (second_t*)term;

    mu_assert("error, second->value->kind != \"Tuple\"",
              match(second->value->kind, "Tuple"));

    tuple_t* tuple = (tuple_t*)second->value;

    mu_assert("error, tuple->first->kind != \"Int\"",
              match(tuple->first->kind, "Int"));
    mu_assert("error, tuple->first->value != 1",
              (*(int*)tuple->first->value) == 1);

    mu_assert("error, tuple->second->kind != \"Int\"",
              match(tuple->second->kind, "Int"));
    mu_assert("error, tuple->second->value != 2",
              (*(int*)tuple->second->value) == 2);

    return 0;
}

static char* test_parse_binary() {
    const char* data =
        "{\"kind\": \"Binary\", \"op\": \"Add\", \"lhs\": {\"kind\": "
        "\"Int\", \"value\": 1}, \"rhs\": {\"kind\": \"Int\", \"value\": "
        "2}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Binary\"", match(term->kind, "Binary"));

    binary_t* binary = (binary_t*)term;

    mu_assert("error, binary->operator != \"Add\"", match(binary->op, "Add"));

    mu_assert("error, binary->left->kind != \"Int\"",
              match(binary->lhs->kind, "Int"));
    mu_assert("error, binary->left->value != 1",
              (*(int*)binary->lhs->value) == 1);

    mu_assert("error, binary->right->kind != \"Int\"",
              match(binary->rhs->kind, "Int"));
    mu_assert("error, binary->right->value != 2",
              (*(int*)binary->rhs->value) == 2);

    return 0;
}

static char* test_parse_if() {
    const char* data =
        "{\"kind\": \"If\", \"condition\": {\"kind\": \"Bool\", \"value\": "
        "true}, \"then\": {\"kind\": \"Int\", \"value\": 1}, \"otherwise\": "
        "{\"kind\": \"Int\", \"value\": 2}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"If\"", match(term->kind, "If"));

    if_t* if_ = (if_t*)term;

    mu_assert("error, if_->condition->kind != \"Bool\"",
              match(if_->condition->kind, "Bool"));
    mu_assert("error, if_->condition->value != true",
              (*(bool*)if_->condition->value) == true);

    mu_assert("error, if_->then->kind != \"Int\"",
              match(if_->then->kind, "Int"));
    mu_assert("error, if_->then->value != 1", (*(int*)if_->then->value) == 1);

    mu_assert("error, if_->otherwise->kind != \"Int\"",
              match(if_->otherwise->kind, "Int"));
    mu_assert("error, if_->otherwise->value != 2",
              (*(int*)if_->otherwise->value) == 2);

    return 0;
}

static char* test_parse_let() {
    const char* data =
        "{\"kind\": \"Let\", \"name\": { \"text\": \"x\" }, \"value\": "
        "{\"kind\": \"Int\", "
        "\"value\": 1}, \"next\": {\"kind\": \"Int\", \"value\": 2}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Let\"", match(term->kind, "Let"));

    let_t* let = (let_t*)term;

    mu_assert("error, let->name != \"x\"", match(let->name->text, "x"));

    mu_assert("error, let->value->kind != \"Int\"",
              match(let->value->kind, "Int"));
    mu_assert("error, let->value->value != 1", (*(int*)let->value->value) == 1);

    mu_assert("error, let->next->kind != \"Int\"",
              match(let->next->kind, "Int"));
    mu_assert("error, let->next->value != 2", (*(int*)let->next->value) == 2);

    return 0;
}

static char* test_parse_var() {
    const char* data = "{\"kind\": \"Var\", \"text\": \"x\"}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Var\"", match(term->kind, "Var"));

    var_t* var = (var_t*)term;

    mu_assert("error, var->text != \"x\"", match(var->text, "x"));

    return 0;
}

static char* test_parse_function() {
    const char* data =
        "{\"kind\": \"Function\", \"parameters\": [{\"text\": \"x\"}, "
        "{\"text\": \"y\"}], \"value\": {\"kind\": \"Int\", \"value\": 1}}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Function\"",
              match(term->kind, "Function"));

    function_t* function = (function_t*)term;

    parameter_t** parameters = (parameter_t**)function->parameters->values;

    mu_assert("error, parameters[0]->text != \"x\"",
              match(parameters[0]->text, "x"));
    mu_assert("error, parameters[1]->text != \"y\"",
              match(parameters[1]->text, "y"));

    mu_assert("error, function->value->kind != \"Int\"",
              match(function->value->kind, "Int"));
    mu_assert("error, function->value->value != 1",
              (*(int*)function->value->value) == 1);

    return 0;
}

static char* test_parse_call() {
    const char* data =
        "{\"kind\": \"Call\", \"callee\": {\"kind\": \"Var\", \"text\": "
        "\"teste\"}, \"arguments\": [{ \"kind\": \"Str\", \"value\": \"hello\" "
        "}]}";
    json_object* root = json_tokener_parse(data);

    term_t* term = parse_expression(root);

    mu_assert("error, term->kind != \"Call\"", match(term->kind, "Call"));

    call_t* call = (call_t*)term;

    mu_assert("error, call->function->kind != \"Function\"",
              match(call->callee->kind, "Var"));

    term_t** arguments = (term_t**)call->arguments->values;

    mu_assert("error, arguments[0]->kind != \"Str\"",
              match(arguments[0]->kind, "Str"));
    mu_assert("error, arguments[0]->value != \"hello\"",
              match(arguments[0]->value, "hello"));

    return 0;
}

const char* letcall =
    "{"
    "    \"name\": \"let.rinha\","
    "    \"expression\": {"
    "        \"kind\": \"Let\","
    "        \"name\": {"
    "            \"text\": \"a\""
    "        },"
    "        \"value\": {"
    "            \"kind\": \"Function\","
    "            \"parameters\": [],"
    "            \"value\": {"
    "                \"kind\": \"Int\","
    "                \"value\": 10"
    "            }"
    "        },"
    "        \"next\": {"
    "            \"kind\": \"Print\","
    "            \"value\": {"
    "                \"kind\": \"Call\","
    "                \"callee\": {"
    "                    \"kind\": \"Var\","
    "                    \"text\": \"a\""
    "                },"
    "                \"arguments\": []"
    "            }"
    "        }"
    "    }"
    "}";

static char* test_eval_let_call_function_no_params() {
    json_object* root = json_tokener_parse(letcall);
    json_object* expression = json_object_object_get(root, "expression");

    term_t* term = parse_expression(expression);
    result_t* res = eval(term);

    mu_assert("error, res->type != Int", match(res->type, "Void"));

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
    mu_run_test(test_eval_let);
    mu_run_test(test_eval_var);
    mu_run_test(test_eval_function_no_params);
    mu_run_test(test_eval_function);
    mu_run_test(test_parse_int);
    mu_run_test(test_parse_bool);
    mu_run_test(test_parse_tuple);
    mu_run_test(test_parse_first);
    mu_run_test(test_parse_second);
    mu_run_test(test_parse_binary);
    mu_run_test(test_parse_if);
    mu_run_test(test_parse_let);
    mu_run_test(test_parse_var);
    mu_run_test(test_parse_function);
    mu_run_test(test_parse_call);
    mu_run_test(test_eval_let_call_function_no_params);
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