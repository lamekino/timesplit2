#pragma once

#include <stddef.h>
#include "Types/Song.h" /* TODO: make this generic, remove this */

#define INITIAL_STACK_CAPACITY 61u
#define STACK_ELEM_T struct Song *

typedef STACK_ELEM_T stack_elem_t;

typedef void (CloseCallback)(void *);

struct Stack {
    stack_elem_t *elems;
    size_t count;
    size_t capacity;
};


stack_elem_t *
stack_create(struct Stack *stk);

stack_elem_t *
stack_push(struct Stack *stk, const stack_elem_t item_ptr, size_t item_size);

stack_elem_t
stack_pop(struct Stack *stk);

stack_elem_t
stack_mod_index(const struct Stack *stk, size_t idx);

void
stack_cleanup(struct Stack *stk, CloseCallback free_elem);
