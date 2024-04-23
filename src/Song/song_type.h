#pragma once

#ifdef SONG_HIDE_IMPL
SONG_HIDE_IMPL
#endif


#include <time.h>
#include <wchar.h>

struct Song {
    time_t timestamp;
    wchar_t *title;
};

typedef const struct Song Song;

void
free_song(void *song);
