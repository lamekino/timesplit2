#include <wchar.h>
#include <time.h>

#include "parser/parse_timestamp.h"
#include "parser/parse_line.h"

#define LENGTH(xs) (sizeof((xs))/sizeof((xs)[0]))
#define WDIGIT(wc) ((wc) - L'0')

static int
parse_timestamp_helper(wchar_t digit, int value, int layer, int max_depth,
        int depth) {
    if (layer != 0 && depth > max_depth) {
        return -1;
    }

    return 10 * value + WDIGIT(digit);
}

/* TODO: this function badly needs refactored */
time_t
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
