#include <wchar.h>
#include <wctype.h>
#include <errno.h>

#include "Parser/wcs_dropwhile.h"

#define WCDIGIT(wc) ((wc) - L'0')

int
iswtrailing(const wint_t wc) {
    return iswspace(wc) || wc == L'-';
}

int
iswtimestamp(const wint_t wc) {
    return iswdigit(wc) || wc == L':';
}

const wchar_t *
wcs_dropwhile(Filter pred, const wchar_t *wcs, size_t *maxlen) {
    size_t i;

    if (!wcs || !maxlen) {
        return NULL;
    }

    for (i = 0; i < *maxlen && *wcs && pred(*wcs); i++) {
        wcs++;
    }

    if (!*wcs) {
        return NULL;
    }

    *maxlen -= i;
    return wcs;
}

const wchar_t *
wcs_dropwhile_r(Filter pred, const wchar_t *wcs, size_t *maxlen) {
    size_t curlen = *maxlen - 1;

    if (!wcs || !maxlen) {
        return NULL;
    }

    while (curlen > 0 && wcs[curlen] && pred(wcs[curlen])) {
        curlen--;
    }

    if (!*wcs) {
        return NULL;
    }

    *maxlen = curlen + 1;
    return wcs;
}

const wchar_t *
wcs_takewhile(Filter pred, const wchar_t *wcs, size_t *maxlen) {
    size_t i = 0;

    if (!wcs || !maxlen) {
        return NULL;
    }

    while (i < *maxlen && wcs[i] && pred(wcs[i])) {
        i++;
    }

    if (!*wcs) {
        return NULL;
    }

    *maxlen = i;
    return wcs;
}

const wchar_t *
wcs_rtrim(const wchar_t *wcs, size_t *maxlen) {
    return wcs_dropwhile_r(iswspace, wcs, maxlen);
}

const wchar_t *
wcs_ltrim(const wchar_t *wcs, size_t *maxlen) {
    return wcs_dropwhile(iswspace, wcs, maxlen);
}

long
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

