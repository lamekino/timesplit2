#include <stdlib.h>

#include "Types/Stack.h"

#if USE_MEMCPY
#define STACKCPY(dest, src, elem_size) memcpy(dest, src, elem_size)
#else
#define STACKCPY(dest, src, _) (*(dest) = *(src), (dest))
#endif

#define INITIAL_STACK_CAPACITY 61u

stack_elem_t *
stack_create(struct Stack *stk) {
    struct Stack created =
        (struct Stack) {
            .capacity = INITIAL_STACK_CAPACITY,
            .elems = calloc(INITIAL_STACK_CAPACITY, sizeof(stack_elem_t)),
        };

    if (!created.elems) {
        return NULL;
    }

    *stk = created;
    return created.elems;
}

static size_t
next_capacity(size_t len, size_t cap) {
    return len > cap ? 2 * cap + 1 : cap;
}

static void *
prepush(void *p, size_t *length, size_t *capacity, size_t elem_size) {
    const size_t cur_cap = *capacity;
    const size_t set_len = *length + 1;
    const size_t set_cap = next_capacity(set_len, cur_cap);

    if (cur_cap < set_cap) {
        p = realloc(p, set_cap * elem_size);
    }

    if (p) {
        *length = set_len;
        *capacity = set_cap;
    }

    return p;
}

#define stack_push(stk, item_ptr) \
    stack_push_item((stk), (item_ptr), sizeof(*stk->elems[0]))

stack_elem_t *
stack_push_item(struct Stack *stk, const stack_elem_t item_ptr,
        size_t item_size) {
    stack_elem_t dup = malloc(item_size);

    stack_elem_t *elems = prepush(stk->elems, &stk->count,
            &stk->capacity, sizeof(stack_elem_t));

    if (!elems || !dup) {
        free(elems);
        free(dup);
        return NULL;
    }

    stk->elems = elems;
    stk->elems[stk->count - 1] = STACKCPY(dup, item_ptr, item_size);

    return &stk->elems[stk->count - 1];
}

stack_elem_t
stack_pop(struct Stack *stk) {
    if (stk->count == 0) {
        return NULL;
    }

    stk->count--;
    return stk->elems[stk->count];
}

stack_elem_t
stack_mod_index(const struct Stack *stk, size_t idx) {
    return stk->elems[idx % stk->count];
}

void
stack_cleanup(struct Stack *stk, CloseCallback free_elem) {
    while (stk->count > 0 && stk->count--) {
        free_elem(stk->elems[stk->count]);
    }

    free(stk->elems);
}

void
stack_reclaim(struct Stack *stk, CloseCallback free_elem) {
    (void) stk;
    (void) free_elem;
}
