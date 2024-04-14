#include <wchar.h>
#include <time.h>

#include "parser/parse_timestamp.h"

#define LENGTH(xs) (sizeof((xs))/sizeof((xs)[0]))
#define WCDIGIT(wc) ((wc) - L'0')

#define HOUR_SECOND(h) (3600 * (h))
#define MIN_SECOND(m) (60 * (m))

#define TS_FIELDS 3

union Timestamp {
    struct {
        int hour;
        int min;
        int sec;
    };

    int items[TS_FIELDS];
};

static int
parse_timestamp_digit(wchar_t digit, int value, int layer, int depth) {
    if (layer != 0 && depth > 2) {
        return -1;
    }

    return 10 * value + WCDIGIT(digit);
}


static int
parse_timestamp_helper(const wchar_t *line, size_t len, size_t pos,
        const int *base, int *cur, size_t depth) {
    const size_t fieldno = cur - base;

    if (pos >= len) {
        return 0;
    }

    if (fieldno >= TS_FIELDS) {
        return -1;
    }

    switch (line[pos]) {
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
        *cur = parse_timestamp_digit(line[pos], *cur, fieldno, depth);
        if (*cur < 0) break;

        pos +=1;
        depth += 1;

        return parse_timestamp_helper(line, len, pos, base, cur, depth);
    case L':':
        depth = 0;
        cur += 1;
        pos += 1;

        return parse_timestamp_helper(line, len, pos, base, cur, depth);
    default:
        break;
    }

    return -1;
}

static int
character_count(wchar_t ch, const wchar_t *ws, size_t len) {
    size_t i;
    int count = 0;

    for (i = 0; i < len; i++) {
        if (ws[i] == ch) count++;
    }

    return count;
}

static int
parse_timestamp_init_layer(const wchar_t *line, size_t len) {
    int count = character_count(L':', line, len);

    switch (count) {
    case 2:
        return 0;
    case 1:
        return 1;
    default:
        break;
    }

    return -1;
}

time_t
parse_timestamp(const wchar_t *line, size_t len) {
    int result, layer;
    union Timestamp ts = {0};

    layer = parse_timestamp_init_layer(line, len);
    if (layer < 0) {
        return -1;
    }

    result =
        parse_timestamp_helper(line, len, 0, ts.items, &ts.items[layer], layer);
    if (result < 0) {
        return -1;
    }

    return ts.sec + MIN_SECOND(ts.min) + HOUR_SECOND(ts.hour);
}
