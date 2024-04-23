#pragma once

#ifdef PARSER_HIDE_IMPL
PARSER_HIDE_IMPL
#endif

#include <wchar.h>
#include <time.h>

time_t
parse_timestamp(const wchar_t *line, size_t len);
