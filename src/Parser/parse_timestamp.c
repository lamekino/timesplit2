#include <wchar.h>
#include <time.h>

#include "Parser/parse_timestamp.h"

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
parse_ts_digit(wchar_t digit, int value, int layer, int digitnum) {
    if (layer != 0 && digitnum >= 2) {
        return -1;
    }

    return 10 * value + WCDIGIT(digit);
}


static int
parse_ts_helper(const wchar_t *line, size_t len, size_t pos,
        const int *base, int *cur, size_t digitnum) {
    const size_t fieldnum = cur - base;

    if (pos >= len) {
        return 0;
    }

    if (fieldnum >= TS_FIELDS) {
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
        *cur = parse_ts_digit(line[pos], *cur, fieldnum, digitnum);
        if (*cur < 0) break;

        digitnum += 1;
        pos +=1;

        return parse_ts_helper(line, len, pos, base, cur, digitnum);
    case L':':
        digitnum = 0;
        pos += 1;
        cur += 1;

        return parse_ts_helper(line, len, pos, base, cur, digitnum);
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
parse_ts_init_layer(const wchar_t *line, size_t len) {
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
    int layer;
    union Timestamp ts = {0};

    layer = parse_ts_init_layer(line, len);
    if (layer < 0) {
        return -1;
    }

    if (parse_ts_helper(line, len, 0, ts.items, &ts.items[layer], 0) < 0) {
        return -1;
    }

    return ts.sec + MIN_SECOND(ts.min) + HOUR_SECOND(ts.hour);
}
