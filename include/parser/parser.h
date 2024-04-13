#ifndef PARSER_H
#define PARSER_H

#include <wchar.h>
#include <time.h>

#include "types/song.h"

#define PARSE_ERROR ((struct Song) { .timestamp = -1 })
#define IS_PARSE_ERROR(song) ((song).timestamp == -1)

struct Song
parse_line(const wchar_t *line_buf, size_t max_len);

#endif /* PARSER_H */
