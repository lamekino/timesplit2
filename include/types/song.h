#ifndef SONG_H
#define SONG_H

#include <time.h>
#include <wchar.h>

/* do something like Either<general, shitty> for a general parse
 * (got artist, song name, timestamp) or a shitty parse, ie just the timestamp
 * and following line */
struct Song {
    time_t timestamp;
    wchar_t *title;
};

void
free_song(void *song);

#endif /* SONG_H */
