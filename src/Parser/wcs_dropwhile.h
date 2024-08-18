#pragma once

#include <stddef.h>
#include <wchar.h>

typedef int (Filter)(const wint_t);

Filter iswtimestamp;
Filter iswtrailing;

const wchar_t *
wcs_dropwhile(Filter pred, const wchar_t *wcs, size_t *maxlen);

const wchar_t *
wcs_dropwhile_r(Filter pred, const wchar_t *wcs, size_t *maxlen);

const wchar_t *
wcs_takewhile(Filter pred, const wchar_t *wcs, size_t *maxlen);

const wchar_t *
wcs_ltrim(const wchar_t *wcs, size_t *maxlen);

const wchar_t *
wcs_rtrim(const wchar_t *wcs, size_t *maxlen);

long
wcs_digit_sum(const wchar_t *digits, size_t len);
