#include <stdio.h>
#include <wchar.h>
#include <wctype.h>

#include "Parser/parse_timestamp.h"

#define WCDIGIT(wc) ((wc) - L'0')

#define HOUR_SECOND(h) (3600 * (h))
#define MIN_SECOND(m) (60 * (m))

#define SEP L':'

typedef int TimestampField;

union Timestamp {
    struct {
        TimestampField hour;
        TimestampField min;
        TimestampField sec;
    };

    TimestampField fields[3];
};

#define TS_FIELDS (sizeof(union Timestamp) / sizeof(TimestampField))

static TimestampField
parse_digit(wchar_t digit, TimestampField value, int valuelen, int fieldno) {
    if (fieldno != 0 && valuelen >= 2) {
        return -1;
    }

    if (!iswdigit(digit)) {
        return -1;
    }

    return 10 * value + WCDIGIT(digit);
}


static int
parse_helper(const wchar_t *line, size_t linelen, size_t linepos,
        const TimestampField *basefield, TimestampField *curfield,
        size_t numdigits) {
    const size_t pos = linepos;
    const size_t len = linelen;

    const size_t fieldpos = curfield - basefield;

    if (linepos >= len || fieldpos >= TS_FIELDS) {
        return 0;
    }

    if (iswdigit(line[linepos])) {
        int nextvalue = parse_digit(line[pos], *curfield, numdigits, fieldpos);
        if (nextvalue < 0) {
            return -1;
        }

        numdigits += 1;
        linepos += 1;
        *curfield = nextvalue;

        return parse_helper(line, len, linepos, basefield, curfield, numdigits);
    }

    if (line[linepos] == SEP) {
        numdigits = 0;
        linepos += 1;
        curfield += 1;

        return parse_helper(line, len, linepos, basefield, curfield, numdigits);
    }

    return -1;
}

static size_t
parse_ts_field_num(const wchar_t *line, size_t len) {
    const size_t MAX_SEPS = TS_FIELDS - 1;

    size_t i;
    size_t sepcount = 0;

    for (i = 0; i < len; i++) {
        if (line[i] == SEP) {
            sepcount++;
        }
    }

    if (!(1 <= sepcount && sepcount <= MAX_SEPS)) {
        return -1;
    }

    return MAX_SEPS - sepcount;
}

time_t
parse_timestamp(const wchar_t *line, size_t len) {
    int fieldno;
    union Timestamp ts = {0};

    fieldno = parse_ts_field_num(line, len);
    if (fieldno < 0) {
        return -1;
    }

    if (parse_helper(line, len, 0, ts.fields, &ts.fields[fieldno], 0) < 0) {
        return -1;
    }

    return ts.sec + MIN_SECOND(ts.min) + HOUR_SECOND(ts.hour);
}
