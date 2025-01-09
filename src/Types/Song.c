#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sndfile.h>

#include "Types/Song.h"
#include "Macro/bounded.h"
#include "Macro/min.h"

void
free_song(void *song) {
    struct Song *as_song = (struct Song *) song;

    free(as_song->title);
    free(as_song);
}

static const char *
fileformat_extension(int format) {
    SF_FORMAT_INFO info = { .format = format };

    if (sf_command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info)) != 0) {
        return NULL;
    }

    return info.extension;
}

int
song_filename(const char *dir, Song *song, int format,
        char *buf, size_t buflen) {
    const char *extension = fileformat_extension(format);

    const long long dirlen = strlen(dir);
    const long long extlen = strlen(extension) + 1;
    const long long titlelen = wcslen(song->title);
    const long long trunclen = buflen - extlen - dirlen;

    int taken, written;

    if (!(BOUNDED(titlelen, 0, INT_MAX) && BOUNDED(trunclen, 0, INT_MAX))) {
        return -1;
    }

    taken = MIN(titlelen, trunclen);
    if (taken < 0) {
        return -1;
    }

    written =
        snprintf(buf, buflen, "%s/%.*ls.%s",
            dir, taken, song->title, extension);
    if (written < 0) {
        return -1;
    }

    return 0;
}

