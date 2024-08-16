#pragma once

#include <stddef.h>
#include <wchar.h>

typedef int (Filter)(const wint_t);

Filter iswtimestamp;

const wchar_t *
wcs_dropwhile(const wchar_t *wcs, size_t *maxlen, Filter pred);

const wchar_t *
wcs_dropwhile_r(const wchar_t *wcs, size_t *maxlen, Filter pred);

const wchar_t *
wcs_ltrim(const wchar_t *wcs, size_t *maxlen);

const wchar_t *
wcs_rtrim(const wchar_t *wcs, size_t *maxlen);
