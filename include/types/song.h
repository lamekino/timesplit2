#ifndef SONG_H
#define SONG_H

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
