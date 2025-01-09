#pragma once

#include <time.h>
#include <wchar.h>

struct Song {
    time_t timestamp;
    wchar_t *title;
};

typedef const struct Song Song;

void
free_song(void *song);

int
song_filename(const char *dir, Song *song, int format,
        char *buf, size_t buflen);
