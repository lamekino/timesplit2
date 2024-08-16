#include <wchar.h>
#include <wctype.h>

#include "Parser/wcs_dropwhile.h"

int
iswtimestamp(const wint_t wc) {
    return iswspace(wc) || iswdigit(wc) || wc == L':';
}

const wchar_t *
wcs_dropwhile(const wchar_t *wcs, size_t *maxlen, Filter pred) {
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
wcs_dropwhile_r(const wchar_t *wcs, size_t *maxlen, Filter p) {
    size_t curlen = *maxlen - 1;

    if (!wcs || !maxlen) {
        return NULL;
    }

    while (curlen > 0 && wcs[curlen] && p(wcs[curlen])) {
        curlen--;
    }

    if (!*wcs) {
        return NULL;
    }

    *maxlen = curlen + 1;
    return wcs;
}

const wchar_t *
wcs_rtrim(const wchar_t *wcs, size_t *maxlen) {
    return wcs_dropwhile_r(wcs, maxlen, iswspace);
}

const wchar_t *
wcs_ltrim(const wchar_t *wcs, size_t *maxlen) {
    return wcs_dropwhile(wcs, maxlen, iswspace);
}
