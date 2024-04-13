#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#define INITIAL_STACK_CAPACITY 15u
#define BUFFER_SIZE 1024

#define PARSE_ERROR ((struct Song) { .timestamp = -1 })
#define IS_PARSE_ERROR(song) ((song).timestamp == -1)

#define WDIGIT(wc) ((wc) - L'0')

#define EXIT_WHEN(p, blame) \
    do { \
        if (p) { \
            perror(blame); \
            exit(EXIT_FAILURE); \
        } \
    } while (0);

typedef void (CloseCallback)(void *);

/* do something like Either<general, shitty> for a general parse
 * (got artist, song name, timestamp) or a shitty parse, ie just the timestamp
 * and following line */
struct Song {
    time_t timestamp;
    wchar_t *title;
};

#define stack_elem_t struct Song *

#if USE_MEMCPY
#define STACKCPY(dest, src, elem_size) memcpy(dest, src, elem_size)
#else
#define STACKCPY(dest, src, _) (*(dest) = *(src), (dest))
#endif

struct Stack {
    stack_elem_t *elems;
    size_t count;
    size_t capacity;
};

stack_elem_t *
stack_create(struct Stack *stk) {
    struct Stack created =
        (struct Stack) {
            .capacity = INITIAL_STACK_CAPACITY,
            .elems = calloc(sizeof(stack_elem_t), INITIAL_STACK_CAPACITY),
        };

    if (!created.elems) {
        return NULL;
    }

    *stk = created;
    return created.elems;
}

static size_t
stack_next_capacity(size_t len, size_t cap) {
    return len > cap ? 2 * cap + 1 : cap;
}

static void *
stack_prepush(void *p, size_t *length, size_t *capacity, size_t elem_size) {
    const size_t cur_cap = *capacity;
    const size_t set_len = *length + 1;
    const size_t set_cap = stack_next_capacity(set_len, cur_cap);

    if (cur_cap < set_cap) {
        p = realloc(p, set_cap * elem_size);
    }

    if (p) {
        *length = set_len;
        *capacity = set_cap;
    }

    return p;
}

stack_elem_t *
stack_push(struct Stack *stk, const stack_elem_t item_ptr, size_t item_size) {
    stack_elem_t dup = malloc(item_size);

    stack_elem_t *elems = stack_prepush(stk->elems, &stk->count,
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
    /* TODO: collect when at ~25% of capacity */
    if (stk->count == 0) {
        return NULL;
    }

    stk->count--;
    return stk->elems[stk->count];
}

void
stack_cleanup(struct Stack *stk, CloseCallback free_elem) {
    while (stk->count > 0 && stk->count--) {
        free_elem(stk->elems[stk->count]);
    }

    free(stk->elems);
}

void
free_song(void *song) {
    struct Song *as_song = (struct Song *) song;

    free(as_song->title);
    free(as_song);
}

int
parse_timestamp_helper(wchar_t digit, int value, int layer, int max_depth,
        int depth) {
    if (layer != 0 && depth > max_depth) {
        return -1;
    }

    return 10 * value + WDIGIT(digit);
}

/* TODO: this function badly needs refactored */
time_t
parse_timestamp(const wchar_t *line, size_t len) {
    /* WARNING: we need to pre-parsed this to know the max number of layers
     * since it could be XX:XX */
    const int coeffs[] = { 3600, 60, 1 };
    const size_t max_layers = sizeof(coeffs);

    int values[sizeof(coeffs)] = {0};
    int *cur = &values[0];

    size_t i;
    size_t layer = 0;
    size_t depth = 0;

    time_t ret = 0;

    for (i = 0; i < len; i++) {
        if (layer >= max_layers) {
            return -1;
        }

        switch (line[i]) {
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9':
        case L'0':
            depth++;
            *cur = parse_timestamp_helper(line[i], *cur, layer, 2, depth);
            break;
        case L':':
            depth = 0; cur++; layer++;
            break;
        default:
            return -1;
        }

        if (*cur == -1) {
            return -1;
        }
    }

    for (i = 0; i < max_layers; i++) {
        ret += coeffs[i] * values[i];
    }

    return ret;
}

struct Song
parse_line_finish(const wchar_t *line_buf, const wchar_t *cursor, size_t delta) {
    size_t span = wcscspn(cursor, L"\n");
    wchar_t *dup = malloc(sizeof(wchar_t) * (span + 1));

    if (!dup) {
        return PARSE_ERROR;
    }

    memcpy(dup, cursor, sizeof(wchar_t) * span);
    dup[span] = L'\0';

    return (struct Song) {
        .timestamp = parse_timestamp(line_buf, delta),
        .title = dup
    };
}


struct Song
parse_line_helper(const wchar_t *line_buf, const wchar_t *cursor,
        size_t delta, size_t max_len) {
    if (delta >= max_len) {
        return PARSE_ERROR;
    }

    delta = cursor - line_buf;

    /* WARN: idk what will happen if ' ' is the final char */
    switch (*cursor) {
    case L'1':
    case L'2':
    case L'3':
    case L'4':
    case L'5':
    case L'6':
    case L'7':
    case L'8':
    case L'9':
    case L'0':
    case L':': return parse_line_helper(line_buf, cursor + 1, delta, max_len);
    case L' ': cursor += 1; /* fall through */
    default:   return parse_line_finish(line_buf, cursor, delta);
    }
}

struct Song
parse_line(const wchar_t *line_buf, size_t max_len) {
    return parse_line_helper(line_buf, line_buf, 0, max_len);
}

int main() {
    size_t i = 0;

    wchar_t line_buf[BUFFER_SIZE] = {0};
    struct Stack data = {0};
    FILE *fp = fopen("./sample-input/MzCEqlPp0L8", "r");

    /* TODO: defer */
    EXIT_WHEN(!fp, "fopen");
    EXIT_WHEN(!setlocale(LC_ALL, "en_US.UTF-8"), "setlocale");
    EXIT_WHEN(!stack_create(&data), "malloc");

    while ((fgetws(line_buf, BUFFER_SIZE, fp))) {
        if (wcsncmp(line_buf, L"\n", BUFFER_SIZE) == 0) continue;

        const struct Song song = parse_line(line_buf, BUFFER_SIZE);

        if (IS_PARSE_ERROR(song)) {
            break;
        }

        if (!stack_push(&data, &song, sizeof(struct Song))) {
            break;
        }
    }

    for (i = 0; i < data.count; i++) {
        struct Song *cur = data.elems[i];
        printf("L\"%ls\": %ld\n", cur->title, cur->timestamp);
    }

    stack_cleanup(&data, free_song);
    fclose(fp);

    return EXIT_SUCCESS;
}
