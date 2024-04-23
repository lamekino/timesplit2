#pragma once

#ifdef PARSER_HIDE_IMPL
PARSER_HIDE_IMPL
#endif

#include <stdio.h>
#include "Song/Song.h"

#define PARSE_ERROR ((struct Song) { .timestamp = -1 })
#define IS_PARSE_ERROR(song) ((song).timestamp == -1)

int
parse_stream(FILE *stream, struct SongList *dest);
