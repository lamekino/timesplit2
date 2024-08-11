#pragma once

#include <wchar.h>
#include <time.h>

#define TS_SEP L':'

time_t
parse_timestamp(const wchar_t *line, size_t len);
