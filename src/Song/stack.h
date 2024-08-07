#pragma once

#include <stddef.h>
#include "Song/song_type.h"

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
stack_push_item(struct Stack *stk, const stack_elem_t item_ptr,
        size_t item_size);

#define stack_push(stk, item_ptr) \
    stack_push_item((stk), (item_ptr), sizeof(*stk->elems[0]))

stack_elem_t
stack_pop(struct Stack *stk);

stack_elem_t
stack_mod_index(const struct Stack *stk, size_t idx);

void
stack_cleanup(struct Stack *stk, CloseCallback free_elem);
