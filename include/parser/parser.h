#ifndef PARSER_H
#define PARSER_H

#include <wchar.h>
#include <time.h>
#include <stdio.h>

#include "types/song.h"
#include "types/stack.h"

#define PARSE_ERROR ((struct Song) { .timestamp = -1 })
#define IS_PARSE_ERROR(song) ((song).timestamp == -1)

int
parse_stream(FILE *stream, struct Stack *dest);

#endif /* PARSER_H */
