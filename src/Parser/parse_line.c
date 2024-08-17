#include <bits/types/time_t.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "Types/Song.h"
#include "Parser/parse_stream.h"
#include "Parser/wcs_dropwhile.h"
#include "Parser/parse_line.h"

#include <stdio.h>

#include <errno.h>

#define TS_SEP L':'
#define WCDIGIT(wc) ((wc) - L'0')

#define HOUR_SECOND(h) (3600 * (h))
#define MIN_SECOND(m) (60 * (m))

typedef int TimestampValue;

enum TimestampField {
    TS_HOUR,
    TS_MIN,
    TS_SEC,

    FIELD_COUNT
};

union Timestamp {
    struct {
        TimestampValue hour;
        TimestampValue min;
        TimestampValue sec;
    };

    TimestampValue fields[FIELD_COUNT];
};

static enum TimestampField
timestamp_field_num(const wchar_t *line, size_t len) {
    const size_t MAX_SEPS = FIELD_COUNT - 1;

    size_t i;
    size_t sepcount = 0;

    for (i = 0; i < len && !iswspace(line[i]); i++) {
        if (line[i] == TS_SEP) {
            sepcount++;
        }
    }

    if (!(1 <= sepcount && sepcount <= MAX_SEPS)) {
        return -1;
    }

    return MAX_SEPS - sepcount;
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

/* TODO: make wcs_foldl */
static long
wcs_digit_sum(const wchar_t *digits, size_t len) {
    long sum = 0;
    size_t i;

    for (i = 0; i < len; i++) {
        if (!iswdigit(digits[i])) {
            errno = EINVAL;
            return -1;
        }

        sum += 10 * sum + WCDIGIT(digits[i]);
    }

    return sum;
}

static time_t
parse_line_timestamp(const wchar_t *timestamp, size_t len) {
    enum TimestampField fieldno;

    union Timestamp ts = {0};

    const int init_field = timestamp_field_num(timestamp, len);

    if (init_field < 0) {
        return -1;
    }

    for (
        fieldno = init_field;
        fieldno < FIELD_COUNT;
        fieldno++
    ) {
        const wchar_t *start = timestamp;

        size_t digitlen = len;
        TimestampValue *fieldcur = &ts.fields[fieldno];

        timestamp = timestamp_next_sep(timestamp, &digitlen, fieldno);
        if (!timestamp) {
            return -1;
        }

        *fieldcur = wcs_digit_sum(start, digitlen);
        if (errno == EINVAL) {
            return -1;
        }

        if (timestamp_field_verify(timestamp, len, digitlen, fieldno) < 0) {
            return -1;
        }

        timestamp += fieldno == TS_HOUR || fieldno == TS_MIN;
    }

    return ts.sec + MIN_SECOND(ts.min) + HOUR_SECOND(ts.hour);
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

    memcpy(song.title, title, sizeof(*song.title) * title_len);
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
