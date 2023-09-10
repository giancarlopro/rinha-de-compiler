#include "interpreter.h"

result_map_t *make_result_map_t(const char *key, result_t *value) {
    result_map_t *result_map = malloc(sizeof(result_map_t));

    result_map->next = NULL;
    result_map->key = key;
    result_map->value = value;

    return result_map;
}

void free_result_map_t(result_map_t *result_map) {
    if (result_map == NULL) return;

    if (result_map->next != NULL) {
        free_result_map_t(result_map->next);
    }

    free_result_t(result_map->value);
    free(result_map);
}

term_map_t *make_term_map_t(const char *key, term_t *value) {
    term_map_t *term_map = malloc(sizeof(term_map_t));

    term_map->next = NULL;
    term_map->key = key;
    term_map->value = value;

    return term_map;
}

void free_term_map_t(term_map_t *term_map) {
    if (term_map == NULL) return;

    if (term_map->next != NULL) {
        free_term_map_t(term_map->next);
    }

    free_term_t(term_map->value);
    free(term_map);
}

stack_t *make_stack_t() {
    stack_t *stack = malloc(sizeof(stack_t));

    stack->next = NULL;
    stack->variables = NULL;
    stack->functions = NULL;

    return stack;
}

void free_stack_t(stack_t *stack) {
    if (stack == NULL) return;

    if (stack->next != NULL) {
        free_stack_t(stack->next);
    }

    free_result_map_t(stack->variables);
    free_term_map_t(stack->functions);
    free(stack);
}

term_t *lookup_term(term_map_t *map, const char *key) {
    if (map == NULL) {
        return NULL;
    } else if (match(map->key, key)) {
        return map->value;
    } else {
        return lookup_term(map->next, key);
    }
}

result_t *lookup_result(result_map_t *map, const char *key) {
    if (map == NULL) {
        return NULL;
    } else if (match(map->key, key)) {
        return map->value;
    } else {
        return lookup_result(map->next, key);
    }
}

stack_t *stack_add(stack_t *stack, stack_t *value) {
    if (stack == NULL) {
        return value;
    }

    stack_t *current = stack;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = value;

    return stack;
}

result_map_t *result_map_add(result_map_t *map, result_map_t *value) {
    if (map == NULL) {
        return value;
    }

    result_map_t *current = map;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = value;

    return map;
}

result_map_t *result_map_replace(result_map_t *map, result_map_t *value) {
    if (map == NULL) {
        return value;
    }

    result_map_t *current = map;
    result_map_t *last = NULL;

    while (current != NULL) {
        if (match(current->key, value->key)) {
            free_result_t(current->value);
            current->value = value->value;

            return map;
        }

        if (current->next == NULL) last = current;

        current = current->next;
    }

    last->next = value;

    return map;
}

term_map_t *term_map_add(term_map_t *map, term_map_t *value) {
    if (map == NULL) {
        return value;
    }

    term_map_t *current = map;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = value;

    return map;
}

int stack_len(stack_t *stack) {
    if (stack == NULL) {
        return 0;
    } else {
        return 1 + stack_len(stack->next);
    }
}