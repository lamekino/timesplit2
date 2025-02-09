#include <bits/types/time_t.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include "Types/Song.h"
#include "Parser/parse_stream.h"
#include "Parser/wcs_dropwhile.h"
#include "Parser/parse_line.h"


#define TS_SEP L':'

#define HOUR_SECONDS 3600
#define MIN_SECONDS 60

typedef int TimestampValue;

enum TimestampField {
    /* OMFG gcc needs to stop making enums unsigned! */
    __gib_sign = -1,

    TS_SEC,
    TS_MIN,
    TS_HOUR,

    FIELD_COUNT
};

static enum TimestampField
timestamp_field_num(const wchar_t *line, size_t len) {
    size_t i;
    size_t sepcount = 0;

    for (i = 0; i < len && !iswspace(line[i]); i++) {
        if (line[i] == TS_SEP) {
            sepcount++;
        }
    }

    if (!(TS_MIN <= sepcount && sepcount <= TS_HOUR)) {
        return -1;
    }

    return sepcount;
}

static const wchar_t *
timestamp_next_sep(const wchar_t *ts, size_t *len, size_t fieldno) {
    const size_t oldlen = *len;
    size_t newlen = *len;

    const wchar_t *next_sep = wcs_dropwhile(iswdigit, ts, &newlen);
    const size_t digitlen = oldlen - newlen;

    if (!next_sep) {
        return NULL;
    }

    if (fieldno != TS_HOUR && digitlen > 2) {
        return NULL;
    }

    *len = digitlen;
    return next_sep;
}

static int
timestamp_field_verify(const wchar_t *timestamp, size_t len,
        size_t fieldlen, enum TimestampField fieldno) {
    if (len - fieldlen <= 0) {
        return -1;
    }

    if (fieldno != TS_SEC && *timestamp != TS_SEP) {
        return -1;
    }

    return 0;
}

static time_t
parse_line_timestamp(const wchar_t *ts_str, size_t len) {
    TimestampValue ts[FIELD_COUNT] = {0};
    enum TimestampField fieldno = timestamp_field_num(ts_str, len);

    if (fieldno < 0) {
        return -1;
    }

    while (fieldno >= TS_SEC) {
        const bool requires_sep = fieldno == TS_HOUR || fieldno == TS_MIN;
        const wchar_t *start = ts_str;

        size_t digitlen = len;
        TimestampValue *fieldcur = &ts[fieldno];

        ts_str = timestamp_next_sep(ts_str, &digitlen, fieldno);
        if (!ts_str) {
            return -1;
        }

        if (timestamp_field_verify(ts_str, len, digitlen, fieldno) < 0) {
            return -1;
        }

        *fieldcur = wcs_digit_sum(start, digitlen);
        if (errno == EINVAL) {
            return -1;
        }

        if (requires_sep && *ts_str != TS_SEP) {
            return -1;
        }

        ts_str += requires_sep;
        fieldno--;
    }

    return ts[TS_HOUR] * HOUR_SECONDS + ts[TS_MIN] * MIN_SECONDS + ts[TS_SEC];
}

static struct Song
parse_line_finish(const wchar_t *timestamp, size_t ts_len,
        const wchar_t *title, size_t title_len) {
    struct Song song = {0};

    if (title <= timestamp) {
        return PARSER_ERROR;
    }

    title = wcs_rtrim(title, &title_len);
    if (!title) {
        return PARSER_ERROR;
    }

    song.timestamp = parse_line_timestamp(timestamp, ts_len);
    if (song.timestamp < 0) {
        return PARSER_ERROR;
    }

    song.title = malloc(sizeof(*song.title) * (title_len + 1));
    if (!song.title) {
        return PARSER_ERROR;
    }

    wcsncpy(song.title, title, title_len);
    song.title[title_len] = '\0';

    return song;
}

struct Song
parse_line(const wchar_t *linebuf) {
    size_t curlen = wcscspn(linebuf, L"\n");

    size_t ts_len = curlen;
    const wchar_t *ts_start = NULL;

    size_t title_len = curlen;
    const wchar_t *title_start = NULL;

    const wchar_t *line_start = wcs_ltrim(linebuf, &curlen);

    if (!line_start) {
        return PARSER_ERROR;
    }

    ts_start = wcs_takewhile(iswtimestamp, line_start, &ts_len);
    if (!ts_start) {
        return PARSER_ERROR;
    }

    title_start = wcs_dropwhile(iswtimestamp, line_start, &title_len);
    if (!title_start) {
        return PARSER_ERROR;
    }

    title_start = wcs_dropwhile(iswtrailing, title_start, &title_len);
    if (!title_start) {
        return PARSER_ERROR;
    }

    return parse_line_finish(ts_start, ts_len, title_start, title_len);
}
