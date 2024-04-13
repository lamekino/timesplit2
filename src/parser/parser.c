#include <wchar.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "types/stack.h"

#define WDIGIT(wc) ((wc) - L'0')
#define LENGTH(xs) (sizeof((xs))/sizeof((xs)[0]))
#define BUFFER_SIZE 1024

static int
parse_timestamp_helper(wchar_t digit, int value, int layer, int max_depth,
        int depth) {
    if (layer != 0 && depth > max_depth) {
        return -1;
    }

    return 10 * value + WDIGIT(digit);
}

/* TODO: this function badly needs refactored */
static time_t
parse_timestamp(const wchar_t *line, size_t len) {
    /* WARNING: we need to pre-parsed this to know the max number of layers
     * since it could be XX:XX */
    const int coeffs[] = { 3600, 60, 1 };
    const size_t max_layers = LENGTH(coeffs);

    int values[LENGTH(coeffs)] = {0};
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

static struct Song
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


static struct Song
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

static struct Song
parse_line(const wchar_t *line_buf, size_t max_len) {
    return parse_line_helper(line_buf, line_buf, 0, max_len);
}

int
parse_stream(FILE *stream, struct Stack *dest) {
    int lineno = 1;
    wchar_t linebuf[BUFFER_SIZE] = {0};

    while ((fgetws(linebuf, BUFFER_SIZE, stream))) {
        struct Song song;

        if (wcsncmp(linebuf, L"\n", BUFFER_SIZE) == 0) continue;

        song = parse_line(linebuf, BUFFER_SIZE);

        if (IS_PARSE_ERROR(song)) {
            return -1;
        }

        if (!stack_push(dest, &song, sizeof(struct Song))) {
            return -1;
        }

        lineno++;
    }

    return lineno;
}
