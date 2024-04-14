#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdio.h>
#include <stddef.h>

#include "types/stack.h"

#define MAX_RESOURCES 128

enum ResourceType {
    NOTHING,
    FILE_POINTER,
    MEMORY,
    STACK
};

struct Resource {
    struct Resource *next;
    struct Resource *prev;

    void *res;
    enum ResourceType type;
};

struct ResoureArena {
    struct Resource resource_arena[MAX_RESOURCES];
    struct Resource *resource_cur;
    size_t resource_count;
};

FILE *
defer_file_resource(struct ResoureArena *arena, FILE **fpp,
        const char *filename, const char *mode);

struct Stack *
defer_stack_resource(struct ResoureArena *arena, struct Stack *stk);

void
free_all_resources(struct ResoureArena *arena);

void
fatal_error(struct ResoureArena *arena, const char *fmt, ...);

#endif /* RESOURCE_H */
