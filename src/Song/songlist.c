#include "Song/song_type.h"
#include "Stack/Stack.h"

STACK_IMPL(SongList, songlist, struct Song *)

stack_elem_t
songlist_mod_index(const struct SongList *songs, size_t idx) {
    return songs->elems[idx % songs->count];
}
