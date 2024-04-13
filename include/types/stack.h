#ifndef STACK_H
#define STACK_H

#include <stddef.h>

#define INITIAL_STACK_CAPACITY 15u
#define stack_elem_t struct Song *

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

void
stack_cleanup(struct Stack *stk, CloseCallback free_elem);

#endif /* STACK_H */
