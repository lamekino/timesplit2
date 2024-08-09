#include <string.h>
#include <sndfile.h>

#include "Types/Song.h"
#include "Audio/soundfile.h"
#include "Audio/extract_song.h"

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
song_filename(Song *song, int format, char *buf, size_t buflen) {
    const char *extension = fileformat_extension(format);

    const size_t ext_len = strlen(extension) + 1;
    const size_t title_len = wcslen(song->title);

    const size_t max_title_len = MIN(title_len, buflen - ext_len);
    const int use_len = (int) max_title_len;

    if (use_len < 0) {
        return -1;
    }

    if (snprintf(buf, buflen, "%.*ls.%s", use_len, song->title, extension) < 0) {
        return -1;
    }

    return 0;
}

static int
extract_section(SoundFile *src, SoundFile *dest, const sf_count_t start,
        const sf_count_t finish, double *songbuf, sf_count_t buflen) {
    const int channels = src->info.channels;
    const int is_last_song = (start > finish);

    sf_count_t pos, copy_size;

    for (
        pos = start;
        pos < finish || is_last_song;
        pos += copy_size / channels
    ) {
        sf_count_t remaining = (finish - pos) * channels;

        copy_size = is_last_song? buflen : MIN(remaining, buflen);

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
extract_song(SoundFile *src, Song *song, sf_count_t position,
        sf_count_t ending, double *songbuf, sf_count_t buflen) {
    const int format = src->info.format;

    struct SoundFile dest = (SoundFile) { .info = src->info };

    char filename[512] = {0};

    if (song_filename(song, format, filename, sizeof(filename)) < 0) {
        return -1;
    }

    if (!soundfile_open(&dest, filename, SFM_WRITE)) {
        return -1;
    }

    if (extract_section(src, &dest, position, ending, songbuf, buflen) < 0) {
        soundfile_close(&dest);
        return -1;
    }

    soundfile_close(&dest);
    return 0;
}
