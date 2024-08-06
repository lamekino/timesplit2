#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "Song/song_type.h"
#include "Parser/parse_stream.h"
#include "Parser/parse_timestamp.h"

static struct Song
parse_line_finish(const wchar_t *line_buf, const wchar_t *cursor, size_t delta) {
    size_t span = wcscspn(cursor, L"\n");
    wchar_t *title = malloc(sizeof(wchar_t) * (span + 1));

    if (!title) {
        return PARSER_ERROR;
    }

    memcpy(title, cursor, sizeof(wchar_t) * span);
    title[span] = L'\0';

    return (struct Song) {
        .timestamp = parse_timestamp(line_buf, delta),
        .title = title
    };
}

static struct Song
parse_line_helper(const wchar_t *line_buf, const wchar_t *cursor,
        size_t max_len) {
    const size_t delta = cursor - line_buf;

    if (delta >= max_len) {
        return PARSER_ERROR;
    }

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
    case L':': return parse_line_helper(line_buf, cursor + 1, max_len);
    case L' ': cursor += 1; /* fall through */
    default:   return parse_line_finish(line_buf, cursor, delta);
    }
}

struct Song
parse_line(const wchar_t *linebuf, size_t max_len) {
    while (*linebuf == L' ' || *linebuf == L'\t') {
        linebuf++;
    }

    if (*linebuf == L'\n') {
        return PARSER_SKIP;
    }

    return parse_line_helper(linebuf, linebuf, max_len);
}

