#include <wchar.h>
#include <wctype.h>

#include "Parser/wcs_dropwhile.h"

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
