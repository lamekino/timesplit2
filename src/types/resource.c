#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "types/stack.h"
#include "types/song.h"
#include "types/resource.h"

/* TODO: public remove single resource with search */
static void
free_resource_ptr(const struct Resource *item) {
    switch (item->type) {
    case NOTHING:
        break;
    case FILE_POINTER:
        fclose(item->res);
        break;
    case MEMORY:
        free(item->res);
        break;
    case STACK:
        stack_cleanup(item->res, free_song);
        break;
    }
}

static struct Resource *
defer_resource(struct ResoureArena *arena, void *ptr, enum ResourceType type) {
    struct Resource *cur = &arena->resource_arena[0];
    struct Resource *prev = NULL;

    if (arena->resource_cur != NULL) {
        prev = arena->resource_cur;

        /* TODO: try to find empty spot */
        arena->resource_cur++;
        if (arena->resource_cur - arena->resource_cur > MAX_RESOURCES) {
            return NULL;
        }

        cur = arena->resource_cur;
    }

    *(cur) =
        (struct Resource) {
            .res = ptr,
            .type = type,
            .next = NULL,
            .prev = prev,
        };

    arena->resource_cur = cur;
    return cur;
}

FILE *
defer_file_resource(struct ResoureArena *arena, FILE **fpp,
        const char *filename, const char *mode) {
    FILE *fp = fopen(filename, mode);
    if (!fp) {
        return NULL;
    }

    if (!defer_resource(arena, fp, FILE_POINTER)) {
        fclose(fp);
        return NULL;
    }

    *fpp = fp;
    return fp;
}

struct Stack *
defer_stack_resource(struct ResoureArena *arena, struct Stack *stk) {
    if (!stack_create(stk)) {
        return NULL;
    }

    if (!defer_resource(arena, stk, STACK)) {
        stack_cleanup(stk, free_song);
        return NULL;
    }

    return stk;
}

void
free_all_resources(struct ResoureArena *arena) {
    size_t i;

    /* TODO: traverse linked list instead */
    for (i = 0; i < MAX_RESOURCES; i++) {
        struct Resource *cur = &arena->resource_arena[i];

        free_resource_ptr(cur);
    }
}

void
report_fatal_error(struct ResoureArena *arena, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    free_all_resources(arena);

    va_end(ap);
}
