#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "Types/Song.h"
#include "Parser/parse_stream.h"
#include "Parser/parse_timestamp.h"

static struct Song
parse_line_finish(const wchar_t *line_buf, const wchar_t *cursor) {
    size_t span;
    wchar_t *title = NULL;


    while (*cursor && !iswalpha(*cursor)) {
        cursor++;
    }

    if (!*cursor) {
        return PARSER_ERROR;
    }

    span = wcscspn(cursor, L"\n");
    title = malloc(sizeof(wchar_t) * (span + 1));

    if (!title) {
        return PARSER_ERROR;
    }

    memcpy(title, cursor, sizeof(wchar_t) * span);
    title[span] = L'\0';

    return (struct Song) {
        .timestamp = parse_timestamp(line_buf, span),
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

    if (iswalpha(*cursor) || *cursor == TS_SEP) {
        return parse_line_helper(line_buf, cursor + 1, max_len);
    }

    if (iswspace(*cursor)) {
        if (*(cursor + 1) == L'\0') {
            return PARSER_ERROR;
        }

        cursor += 1;

    }

    return parse_line_finish(line_buf, cursor);
}

struct Song
parse_line(const wchar_t *linebuf, size_t max_len) {
    while (*linebuf && iswspace(*linebuf)) {
        linebuf++;
    }

    if (*linebuf == L'\n') {
        return PARSER_SKIP;
    }

    return parse_line_helper(linebuf, linebuf, max_len);
}

