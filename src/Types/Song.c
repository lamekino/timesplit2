#include <stdlib.h>

#include "Types/Song.h"

void
free_song(void *song) {
    struct Song *as_song = (struct Song *) song;

    free(as_song->title);
    free(as_song);
}
