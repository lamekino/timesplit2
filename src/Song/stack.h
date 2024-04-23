#pragma once

#include <stddef.h>
#include "Song/song_type.h"

#define INITIAL_STACK_CAPACITY 61u
#define STACK_ELEM_T struct Song *

typedef STACK_ELEM_T stack_elem_t;

typedef void (CloseCallback)(void *);

struct SongList {
    stack_elem_t *elems;
    size_t count;
    size_t capacity;
};


stack_elem_t *
songlist_stack_create(struct SongList *stk);

stack_elem_t *
songlist_push_item(struct SongList *stk, const stack_elem_t item_ptr,
        size_t item_size);

#ifndef USE_MEMCPY
#define songlist_push(stk, item_ptr) \
    songlist_push_item((stk), (item_ptr), sizeof(*stk->elems[0]))
#else
#define songlist_push(stk, item_ptr, item_size) \
    songlist_push_item((stk), (item_ptr), (item_size))
#endif

stack_elem_t
songlist_pop(struct SongList *stk);

stack_elem_t
songlist_mod_index(const struct SongList *stk, size_t idx);

void
songlist_cleanup(struct SongList *stk, CloseCallback free_elem);
