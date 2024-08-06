#pragma once

#include <stdio.h>
#include <limits.h>
#include "Song/Song.h"

enum ParserState {
    __ParserState_start = INT_MIN,

    STATE_ERROR,
    STATE_SKIP,
};

#define PARSER_ERROR ((struct Song) { .timestamp = STATE_ERROR })
#define PARSER_SKIP ((struct Song) { .timestamp = STATE_SKIP })

#define IS_PARSER_ERROR(song) ((song).timestamp == STATE_ERROR)
#define IS_PARSER_SKIP(song) ((song).timestamp == STATE_SKIP)

int
parse_stream(FILE *stream, struct SongList *dest);
