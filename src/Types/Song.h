#ifndef SONG_H
#define SONG_H

#include <stdlib.h>
#include <time.h>
#include <wchar.h>

struct Song {
    time_t timestamp;
    wchar_t *title;
};

typedef const struct Song Song;

void
free_song(void *song);

#endif /* SONG_H */


#ifdef SONG_IMPL

void
free_song(void *song) {
    struct Song *as_song = (struct Song *) song;

    free(as_song->title);
    free(as_song);
}

#endif /* SONG_IMPL */
