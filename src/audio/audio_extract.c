#include <string.h>
#include <sndfile.h>

#include "types/song.h"
#include "audio/audio_file.h"
#include "audio/audio_extract.h"

#define MIN(x, y) ((x) > (y) ? (y) : (x))

static const char *
fileformat_extension(int format) {
    SF_FORMAT_INFO info = { .format = format };

    if (sf_command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info)) != 0) {
        return NULL;
    }

    return info.extension;
}

static int
song_filename(Song *song, int format, char *buf, size_t len) {
    const char *extension = fileformat_extension(format);

    /* TODO: account for when |extension| + |title| > len, use
     * %.*ls + a size param to say how much of title to use */
    if (snprintf(buf, len, "%ls.%s", song->title, extension) < 0) {
        return -1;
    }

    return 0;
}

static int
extract_section(AudioFile *src, AudioFile *dest, const sf_count_t start,
        const sf_count_t finish, double *songbuf, sf_count_t buflen) {
    const int channels = src->info.channels;
    const int is_last_song = (start > finish);

    sf_count_t pos, copy_size;

    for (
        pos = start;
        pos < finish || is_last_song;
        pos += copy_size / channels
    ) {
        copy_size = is_last_song? buflen : MIN(finish - pos, buflen);

        if (sf_read_double(src->file, songbuf, copy_size) <= 0) {
            return is_last_song? 0 : -1;
        }

        if (sf_write_double(dest->file, songbuf, copy_size) != copy_size) {
            return -1;
        }
    }

    return 0;
}

int
audio_extract(AudioFile *src, Song *song, sf_count_t position,
        sf_count_t ending, double *songbuf, sf_count_t buflen) {
    const int format = src->info.format;

    char filename[512] = {0};
    struct AudioFile dest = (AudioFile) { .info = src->info };

    if (song_filename(song, format, filename, sizeof(filename)) < 0) {
        return -1;
    }

    if (!audio_open(&dest, filename, SFM_WRITE)) {
        return -1;
    }

    if (extract_section(src, &dest, position, ending, songbuf, buflen) < 0) {
        audio_close(&dest);
        return -1;
    }

    audio_close(&dest);
    return 0;
}
