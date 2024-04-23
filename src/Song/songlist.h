#pragma once

#ifdef SONG_HIDE_IMPL
SONG_HIDE_IMPL
#endif

#include "Song/song_type.h"

#define STACK_HEADER_FILE
#include "Stack/Stack.h"

STACK_SIGNATURES(SongList, songlist, struct Song *)

#undef STACK_HEADER_FILE

stack_elem_t
songlist_mod_index(const struct SongList *songs, size_t idx);
