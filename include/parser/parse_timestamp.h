#ifndef PARSE_TIMESTAMP_H
#define PARSE_TIMESTAMP_H

#include <wchar.h>
#include <time.h>

time_t
parse_timestamp(const wchar_t *line, size_t len);

#endif /* PARSE_TIMESTAMP_H */
