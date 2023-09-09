#include "interpreter.h"

#include <stdio.h>
#include <string.h>

term_t *parse_expression(json_object *expression) {
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
    }
}

result_t *print(term_t *root) {
    result_t *res = eval(root);

    if (match(root->kind, "Str")) {
        printf("%s", (char *)res->value);
    } else if (match(root->kind, "Int")) {
        printf("%d", *(int *)res->value);
    } else if (match(root->kind, "Bool")) {
        bool value = *(bool *)res->value;
        if (value) {
            printf("true");
        } else {
            printf("false");
        }
    } else if (match(root->kind, "Tuple")) {
        tuple_t *tuple = (tuple_t *)root;

        printf("(");
        print(tuple->first);
        printf(", ");
        print(tuple->second);
        printf(")");
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
            runtime_error("This operation must be applied to integers");
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
    }
}

result_t *eval(term_t *root) {
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

        if (*(int *)condition->value) {
            return eval(if_term->then);
        } else {
            return eval(if_term->otherwise);
        }
    }
}
