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
parse_digit(wchar_t digit, TimestampField value, int valuelen, int fieldnum) {
    if (fieldnum != 0 && valuelen >= 2) {
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
    const size_t fieldpos = curfield - basefield;

    if (linepos >= linelen) {
        return 0;
    }

    if (fieldpos >= TS_FIELDS) {
        return -1;
    }

    /* and if this where a map of lambdas they'd call it modern! */
    switch (line[linepos]) {
    case L'1': case L'2': case L'3': case L'4': case L'5':
    case L'6': case L'7': case L'8': case L'9': case L'0':
        goto CLOSURE_DIGIT;
    case SEP:
        goto CLOSURE_SEP;
    default:
        break;
    }

    return -1;

CLOSURE_DIGIT: {
    int nextvalue =
        parse_digit(line[linepos], *curfield, numdigits, fieldpos);

    if (nextvalue < 0) {
        return -1;
    }

    *curfield = nextvalue;
    linepos += 1;
    numdigits += 1;

    return parse_helper(line, linelen, linepos, basefield, curfield, numdigits);
}

CLOSURE_SEP: {
    curfield += 1;
    linepos += 1;
    numdigits = 0;

    return parse_helper(line, linelen, linepos, basefield, curfield, numdigits);
}

}

static size_t
parse_ts_field_num(const wchar_t *line, size_t len) {
    size_t i;
    size_t fieldno = TS_FIELDS;

    for (i = 0; i < len; i++) {
        if (line[i] == SEP) {
            fieldno--;
        }
    }

    if (!(1 <= fieldno && fieldno <= TS_FIELDS - 1)) {
        return -1;
    }

    return fieldno;
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
