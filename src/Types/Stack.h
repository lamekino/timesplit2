#pragma once

#include "Types/Song.h"
#define STACK_T struct Song *

typedef void (CloseCallback)(void *);

typedef STACK_T stack_elem_t;

struct Stack {
    stack_elem_t *elems;
    size_t count;
    size_t capacity;
};

stack_elem_t *
stack_create(struct Stack *stk);

#define stack_push(stk, item_ptr) \
    stack_push_item((stk), (item_ptr), sizeof(*stk->elems[0]))

stack_elem_t
stack_pop(struct Stack *stk);

stack_elem_t *
stack_push_item(struct Stack *stk, const stack_elem_t item_ptr,
        size_t item_size);

stack_elem_t
stack_mod_index(const struct Stack *stk, size_t idx);

void
stack_cleanup(struct Stack *stk, CloseCallback free_elem);

void
stack_reclaim(struct Stack *stk, CloseCallback free_elem);
