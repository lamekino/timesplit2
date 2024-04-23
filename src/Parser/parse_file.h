#pragma once

#ifdef PARSER_HIDE_IMPL
PARSER_HIDE_IMPL
#endif

#include "Song/Song.h"

int
parse_file(const char *filepath, struct SongList *dest);
