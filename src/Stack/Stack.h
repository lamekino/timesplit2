#pragma once

#define INITIAL_STACK_CAPACITY 61u

#if STACK_USE_MEMCPY
#define STACKCPY(dest, src, elem_size) memcpy(dest, src, elem_size)
#else
#define STACKCPY(dest, src, _) (*(dest) = *(src), (dest))
#endif

typedef void (CloseCallback)(void *);

#ifdef STACK_HEADER_FILE
#include <stddef.h>

#define STACK_SIGNATURES(stack_name, prefix, elem_t) \
    typedef elem_t stack_elem_t; \
\
    struct stack_name { \
        stack_elem_t *elems; \
        size_t count; \
        size_t capacity; \
    }; \
\
    stack_elem_t * \
    prefix##_stack_create(struct stack_name *stk); \
\
    stack_elem_t * \
    prefix##_push(struct stack_name *stk, const elem_t item_ptr); \
\
    stack_elem_t \
    prefix##_pop(struct stack_name *stk); \
\
    void \
    prefix##_stack_cleanup(struct stack_name *stk, CloseCallback free_elem);
#else
#include <stdlib.h>

#define STACK_IMPL(stack_name, prefix, elem_t) \
    typedef elem_t stack_elem_t; \
\
    struct stack_name { \
        stack_elem_t *elems; \
        size_t count; \
        size_t capacity; \
    }; \
\
    stack_elem_t * \
    prefix##_stack_create(struct stack_name *stk) { \
        struct stack_name created = \
            (struct stack_name) { \
                .capacity = INITIAL_STACK_CAPACITY, \
                .elems = calloc(sizeof(stack_elem_t), INITIAL_STACK_CAPACITY), \
            }; \
\
        if (!created.elems) { \
            return NULL; \
        } \
\
        *stk = created; \
        return created.elems; \
    } \
\
    static size_t \
    next_capacity(size_t len, size_t cap) { \
        return len > cap ? 2 * cap + 1 : cap; \
    } \
\
    static void * \
    prepush(void *p, size_t *length, size_t *capacity, size_t elem_size) { \
        const size_t cur_cap = *capacity; \
        const size_t set_len = *length + 1; \
        const size_t set_cap = next_capacity(set_len, cur_cap); \
\
        if (cur_cap < set_cap) { \
            p = realloc(p, set_cap * elem_size); \
        } \
\
        if (p) { \
            *length = set_len; \
            *capacity = set_cap; \
        } \
\
        return p; \
    } \
\
    stack_elem_t * \
    prefix##_push(struct stack_name *stk, const stack_elem_t item_ptr) { \
        stack_elem_t dup = malloc(sizeof(stack_elem_t)); \
\
        stack_elem_t *elems = prepush(stk->elems, &stk->count, \
                &stk->capacity, sizeof(stack_elem_t)); \
\
        if (!elems || !dup) { \
            free(elems); \
            free(dup); \
            return NULL; \
        } \
\
        stk->elems = elems; \
        stk->elems[stk->count - 1] = \
            STACKCPY(dup, item_ptr, sizeof(stack_elem_t)); \
\
        return &stk->elems[stk->count - 1]; \
    } \
\
    stack_elem_t \
    prefix##_pop(struct stack_name *stk) { \
        if (stk->count == 0) { \
            return NULL; \
        } \
\
        stk->count--; \
        return stk->elems[stk->count]; \
    } \
    void \
    prefix##_stack_cleanup(struct stack_name *stk, CloseCallback free_elem) { \
        while (stk->count > 0 && stk->count--) { \
            free_elem(stk->elems[stk->count]); \
        } \
\
        free(stk->elems); \
    }
#endif
