#include <stdlib.h>

#include "Song/song_type.h"

void
free_song(void *song) {
    struct Song *as_song = (struct Song *) song;

    free(as_song->title);
    free(as_song);
}
