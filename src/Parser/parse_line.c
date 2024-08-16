#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "Types/Song.h"
#include "Parser/parse_stream.h"
#include "Parser/parse_timestamp.h"
#include "Parser/wcs_dropwhile.h"

static struct Song
parse_line_finish(const wchar_t *timestamp, const wchar_t *title) {
    size_t eol = wcscspn(title, L"\n\0");

    struct Song song = {0};

    if (title <= timestamp) {
        return PARSER_ERROR;
    }

    title = wcs_rtrim(title, &eol);
    if (!title) {
        return PARSER_ERROR;
    }

    song.timestamp = parse_timestamp(timestamp, eol);
    if (song.timestamp < 0) {
        return PARSER_ERROR;
    }

    song.title = malloc(sizeof(*song.title) * (eol + 1));
    if (!song.title) {
        return PARSER_ERROR;
    }

    memcpy(song.title, title, sizeof(*song.title) * eol);
    song.title[eol] = '\0';

    return song;
}

struct Song
parse_line(const wchar_t *linebuf, size_t maxlen) {
    const wchar_t *ts_start = NULL;
    const wchar_t *title_start = NULL;

    ts_start = wcs_ltrim(linebuf, &maxlen);
    if (!ts_start) {
        return PARSER_ERROR;
    }

    title_start = wcs_dropwhile(ts_start, &maxlen, iswtimestamp);
    if (!title_start) {
        return PARSER_ERROR;
    }

    return parse_line_finish(ts_start, title_start);
}
