#include "interpreter.h"

#include <stdio.h>
#include <string.h>

stack_t *stack = NULL;

term_t *parse_expression(json_object *expression) {
    if (expression == NULL) {
        runtime_error("Syntax error");
    }

    const char *kind =
        json_object_get_string(json_object_object_get(expression, "kind"));

    if (match(kind, "Print")) {
        return (term_t *)make_print_t(
            parse_expression(json_object_object_get(expression, "value")));
    } else if (match(kind, "Str")) {
        return (term_t *)make_str_t(json_object_get_string(
            json_object_object_get(expression, "value")));
    } else if (match(kind, "Int")) {
        return (term_t *)make_int_t(
            json_object_get_int(json_object_object_get(expression, "value")));
    } else if (match(kind, "Bool")) {
        return (term_t *)make_bool_t(json_object_get_boolean(
            json_object_object_get(expression, "value")));
    } else if (match(kind, "Tuple")) {
        return (term_t *)make_tuple_t(
            parse_expression(json_object_object_get(expression, "first")),
            parse_expression(json_object_object_get(expression, "second")));
    } else if (match(kind, "First")) {
        return (term_t *)make_first_t(
            parse_expression(json_object_object_get(expression, "value")));
    } else if (match(kind, "Second")) {
        return (term_t *)make_second_t(
            parse_expression(json_object_object_get(expression, "value")));
    } else if (match(kind, "Binary")) {
        return (term_t *)make_binary_t(
            json_object_get_string(json_object_object_get(expression, "op")),
            parse_expression(json_object_object_get(expression, "lhs")),
            parse_expression(json_object_object_get(expression, "rhs")));
    } else if (match(kind, "If")) {
        return (term_t *)make_if_t(
            parse_expression(json_object_object_get(expression, "condition")),
            parse_expression(json_object_object_get(expression, "then")),
            parse_expression(json_object_object_get(expression, "otherwise")));
    } else if (match(kind, "Let")) {
        return (term_t *)make_let_t(
            make_parameter_t(json_object_get_string(json_object_object_get(
                json_object_object_get(expression, "name"), "text"))),
            parse_expression(json_object_object_get(expression, "value")),
            parse_expression(json_object_object_get(expression, "next")));
    } else if (match(kind, "Var")) {
        return (term_t *)make_var_t(
            json_object_get_string(json_object_object_get(expression, "text")));
    } else if (match(kind, "Function")) {
        json_object *parameters =
            json_object_object_get(expression, "parameters");

        size_t length = json_object_array_length(parameters);

        parameter_t **params = NULL;
        if (length > 0) {
            params = malloc(sizeof(parameter_t *) * length);

            for (int i = 0; i < length; i++) {
                params[i] = make_parameter_t(
                    json_object_get_string(json_object_object_get(
                        json_object_array_get_idx(parameters, i), "text")));
            }
        }

        return (term_t *)make_function_t(
            make_array_t(length, (void **)params),
            parse_expression(json_object_object_get(expression, "value")));
    } else if (match(kind, "Call")) {
        json_object *arguments =
            json_object_object_get(expression, "arguments");

        size_t length = json_object_array_length(arguments);

        term_t **args = NULL;

        if (length > 0) {
            args = malloc(sizeof(term_t *) * length);

            for (int i = 0; i < length; i++) {
                args[i] =
                    parse_expression(json_object_array_get_idx(arguments, i));
            }
        }

        return (term_t *)make_call_t(
            parse_expression(json_object_object_get(expression, "callee")),
            make_array_t(length, (void **)args));
    } else {
        runtime_error("Unknown term kind: %s", kind);
    }
}

result_t *print(term_t *root) {
    result_t *res = eval(root);

    if (match(res->type, "Str")) {
        printf("%s", (char *)res->value);
    } else if (match(res->type, "Int")) {
        printf("%d", *(int *)res->value);
    } else if (match(res->type, "Bool")) {
        bool value = *(bool *)res->value;
        if (value) {
            printf("true");
        } else {
            printf("false");
        }
    } else if (match(res->type, "Tuple")) {
        tuple_t *tuple = (tuple_t *)root;

        printf("(");
        print(tuple->first);
        printf(", ");
        print(tuple->second);
        printf(")");
    } else {
        runtime_error("Print for kind %s not implemented", res->type);
    }

    return make_result_t(NULL, "Void");
}

char *as_str(result_t *result) {
    if (match(result->type, "Int")) {
        char *data = malloc(20);
        sprintf(data, "%d", *(int *)result->value);
        return data;
    } else if (match(result->type, "Str")) {
        return (char *)result->value;
    } else {
        runtime_error("Cannot convert %s to string", result->type);
    }
}

result_t *eval_binary(binary_t *binary) {
    result_t *left = eval(binary->lhs);
    result_t *right = eval(binary->rhs);

    if (match(binary->op, "Sub") || match(binary->op, "Mul") ||
        match(binary->op, "Div") || match(binary->op, "Rem") ||
        match(binary->op, "Lt") || match(binary->op, "Gt") ||
        match(binary->op, "Lte") || match(binary->op, "Gte")) {
        if (!match(left->type, "Int") || !match(right->type, "Int")) {
            runtime_error("%s operation must be applied to integers",
                          binary->op);
        }
    }

    if (match(binary->op, "Add")) {
        if (match(left->type, "Int") && match(right->type, "Int")) {
            int *buff = malloc(sizeof(int));
            *buff = *(int *)left->value + *(int *)right->value;

            return make_result_t((void *)buff, "Int");
        } else if (match(left->type, "Str") || match(right->type, "Str")) {
            char *left_str = as_str(left);
            char *right_str = as_str(right);

            char *result = malloc(strlen(left_str) + strlen(right_str) + 1);

            strcpy(result, left_str);
            strcat(result, right_str);

            return make_result_t((void *)result, "Str");
        } else {
            runtime_error("Cannot add %s and %s", left->type, right->type);
        }
    } else if (match(binary->op, "Sub")) {
        int *buff = malloc(sizeof(int));

        *buff = *(int *)left->value - *(int *)right->value;

        return make_result_t((void *)buff, "Int");
    } else if (match(binary->op, "Mul")) {
        int *buff = malloc(sizeof(int));

        *buff = *(int *)left->value * *(int *)right->value;

        return make_result_t((void *)buff, "Int");
    } else if (match(binary->op, "Div")) {
        int *buff = malloc(sizeof(int));

        *buff = *(int *)left->value / *(int *)right->value;

        return make_result_t((void *)buff, "Int");
    } else if (match(binary->op, "Rem")) {
        int *buff = malloc(sizeof(int));

        *buff = *(int *)left->value % *(int *)right->value;

        return make_result_t((void *)buff, "Int");
    } else if (match(binary->op, "Lt")) {
        bool *buff = malloc(sizeof(bool));

        *buff = *(int *)left->value < *(int *)right->value;

        return make_result_t((void *)buff, "Bool");
    } else if (match(binary->op, "Gt")) {
        bool *buff = malloc(sizeof(bool));

        *buff = *(int *)left->value > *(int *)right->value;

        return make_result_t((void *)buff, "Bool");
    } else if (match(binary->op, "Lte")) {
        bool *buff = malloc(sizeof(bool));

        *buff = *(int *)left->value <= *(int *)right->value;

        return make_result_t((void *)buff, "Bool");
    } else if (match(binary->op, "Gte")) {
        bool *buff = malloc(sizeof(bool));

        *buff = *(int *)left->value >= *(int *)right->value;

        return make_result_t((void *)buff, "Bool");
    } else if (match(binary->op, "Eq")) {
        bool *buff = malloc(sizeof(bool));

        if (match(left->type, "Int") && match(right->type, "Int")) {
            *buff = *(int *)left->value == *(int *)right->value;
        } else if (match(left->type, "Str") && match(right->type, "Str")) {
            *buff = strcmp((char *)left->value, (char *)right->value) == 0;
        } else if (match(left->type, "Bool") && match(right->type, "Bool")) {
            *buff = *(bool *)left->value == *(bool *)right->value;
        }

        return make_result_t((void *)buff, left->type);
    } else if (match(binary->op, "Neq")) {
        bool *buff = malloc(sizeof(bool));

        if (match(left->type, "Int") && match(right->type, "Int")) {
            *buff = *(int *)left->value != *(int *)right->value;
        } else if (match(left->type, "Str") && match(right->type, "Str")) {
            *buff = strcmp((char *)left->value, (char *)right->value) != 0;
        } else if (match(left->type, "Bool") && match(right->type, "Bool")) {
            *buff = *(bool *)left->value != *(bool *)right->value;
        }

        return make_result_t((void *)buff, left->type);
    } else if (match(binary->op, "And")) {
        bool *buff = malloc(sizeof(bool));

        *buff = *(bool *)left->value && *(bool *)right->value;

        return make_result_t((void *)buff, "Bool");
    } else if (match(binary->op, "Or")) {
        bool *buff = malloc(sizeof(bool));

        *buff = *(bool *)left->value || *(bool *)right->value;

        return make_result_t((void *)buff, "Bool");
    } else {
        runtime_error("Unknown binary operator %s", binary->op);
    }
}

void print_stack_variables() {
    int stack_size = stack_len(stack);

    if (stack->variables != NULL) {
        result_map_t *variables = stack->variables;

        while (variables != NULL) {
            printf("[%d] Variable -> %s = %d\n", stack_size, variables->key,
                   *(int *)variables->value->value);

            variables = variables->next;
        }
    }

    if (stack->functions != NULL) {
        term_map_t *functions = stack->functions;

        while (functions != NULL) {
            printf("[%d] Function -> %s\n", stack_size, functions->key);

            functions = functions->next;
        }
    }
}

void add_stack() {
    stack_t *new_stack = make_stack_t();

    stack = stack_add(stack, new_stack);
}

void pop_stack() {
    stack = stack->parent;
    stack->next = NULL;
}

void push_variable(const char *key, result_t *value) {
    stack->variables =
        result_map_add(stack->variables, make_result_map_t(key, value));
}

void push_function(const char *key, term_t *value) {
    if (stack == NULL) {
        stack = make_stack_t();
    }

    stack_t *root = stack;

    while (root->parent != NULL) {
        root = root->parent;
    }

    root->functions =
        term_map_add(root->functions, make_term_map_t(key, value));
}

void clean() {
    free_stack_t(stack);
    stack = NULL;
}

function_t *lookup_function(const char *name) {
    stack_t *root = stack;

    while (root->parent != NULL) {
        root = root->parent;
    }

    return (function_t *)lookup_term(root->functions, name);
}

result_t *lookup_variable(const char *name) {
    stack_t *root = stack;
    result_t *result = NULL;

    do {
        result = lookup_result(root->variables, name);

        if (result != NULL) {
            return result;
        }

        root = root->parent;
    } while (root != NULL);

    return NULL;
}

result_t *eval(term_t *root) {
    if (stack == NULL) {
        stack = make_stack_t();
    }

    if (match(root->kind, "Print")) {
        return print((term_t *)root->value);
    } else if (match(root->kind, "Str") || match(root->kind, "Int") ||
               match(root->kind, "Bool")) {
        return make_result_t(root->value, root->kind);
    } else if (match(root->kind, "Tuple")) {
        tuple_t *tuple = (tuple_t *)root;

        return make_result_t(
            (void *)make_runtime_tuple_t(eval(tuple->first)->value,
                                         eval(tuple->second)->value),
            "Tuple");
    } else if (match(root->kind, "First")) {
        tuple_t *tuple = (tuple_t *)root->value;

        if (!match(tuple->kind, "Tuple")) {
            runtime_error("first needs to be invoked with a tuple");
        }

        runtime_tuple_t *runtime_tuple =
            (runtime_tuple_t *)eval(root->value)->value;
        return make_result_t(runtime_tuple->first, tuple->first->kind);
    } else if (match(root->kind, "Second")) {
        tuple_t *tuple = (tuple_t *)root->value;

        if (!match(tuple->kind, "Tuple")) {
            runtime_error("second needs to be invoked with a tuple");
        }

        runtime_tuple_t *runtime_tuple =
            (runtime_tuple_t *)eval(root->value)->value;
        return make_result_t(runtime_tuple->second, tuple->second->kind);
    } else if (match(root->kind, "Binary")) {
        return eval_binary((binary_t *)root);
    } else if (match(root->kind, "If")) {
        if_t *if_term = (if_t *)root;

        result_t *condition = eval(if_term->condition);

        if (*(bool *)condition->value) {
            return eval(if_term->then);
        } else {
            return eval(if_term->otherwise);
        }
    } else if (match(root->kind, "Let")) {
        let_t *let = (let_t *)root;

        if (match(let->value->kind, "Function")) {
            push_function(let->name->text, let->value);
        } else {
            result_t *result = eval(let->value);

            push_variable(let->name->text, result);
        }

        if (let->next != NULL) {
            return eval(let->next);
        }

        return make_result_t(NULL, "Void");
    } else if (match(root->kind, "Var")) {
        var_t *var = (var_t *)root;

        result_t *result = lookup_variable(var->text);

        if (result == NULL) {
            runtime_error("Unknown variable %s", var->text);
        }

        return result;
    } else if (match(root->kind, "Function")) {
        return make_result_t(NULL, "Void");
    } else if (match(root->kind, "Call")) {
        call_t *call = (call_t *)root;
        function_t *function = NULL;

        if (match(call->callee->kind, "Var")) {
            function = lookup_function(((var_t *)call->callee)->text);
        }

        if (function == NULL) {
            runtime_error("Unknown function %s", ((var_t *)call->callee)->text);
        }

        if (call->arguments != NULL && function->parameters != NULL &&
            len(call->arguments) != len(function->parameters)) {
            runtime_error("Wrong number of arguments");
        }

        variable_t **variables = NULL;

        if (call->arguments != NULL && function->parameters != NULL) {
            size_t length = call->arguments->length;

            variables = malloc(sizeof(variable_t *) * length);

            parameter_t **parameters =
                (parameter_t **)function->parameters->values;
            term_t **arguments = (term_t **)call->arguments->values;

            for (int i = 0; i < length; i++) {
                const char *parameter = parameters[i]->text;

                result_t *result = eval(arguments[i]);

                variables[i] = make_variable_t(parameter, result);
            }
        }

        add_stack();

        if (call->arguments != NULL) {
            for (int i = 0; i < call->arguments->length; i++) {
                push_variable(variables[i]->key, variables[i]->value);
            }
        }

        result_t *result = eval(function->value);

        pop_stack();

        return result;
    } else {
        runtime_error("Unknown term kind %s", root->kind);
    }
}
