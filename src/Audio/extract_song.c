#include <string.h>
#include <limits.h>
#include <sndfile.h>

#include "App/AppOutput.h"
#include "Types/Song.h"
#include "Audio/AudioFile.h"
#include "Audio/extract_song.h"
#include "Debug/assert.h"

#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define BOUNDED(x, lo, hi) ((lo) <= (x) && (x) <= (hi))

static const char *
fileformat_extension(int format) {
    SF_FORMAT_INFO info = { .format = format };

    if (sf_command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info)) != 0) {
        return NULL;
    }

    return info.extension;
}

static int
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

static int
extract_section(AudioFile *src, AudioFile *dest, const sf_count_t start,
        const sf_count_t finish, double *songbuf, sf_count_t buflen) {
    const int channels = src->info.channels;
    /* BUG: we need to account for incorrect input, ie if the mix is shorter
     * than the timestamps given */
    const int is_last_song = (start > finish);

    sf_count_t pos, copy_size;

    for (
        pos = start;
        pos < finish || is_last_song;
        pos += copy_size / channels
    ) {
        sf_count_t remaining = (finish - pos) * channels;

        /* TODO: precompute length so that we don't need an extra case for the
         * last song in the mix */
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
extract_song(AudioFile *src, AppOutput *out,
        double *songbuf, sf_count_t buflen) {
    const int format = src->info.format;

    struct AudioFile dest = (AudioFile) { .info = src->info };

    char fname[512] = {0};

    const char *outdir = out->output_directory;
    const sf_count_t start = out->start;
    const sf_count_t end = out->end;

    if (!outdir) {
        outdir = "./";
    }

    if (song_filename(outdir, out->song, format, fname, sizeof(fname)) < 0) {
        goto FAIL;
    }

    if (!audiofile_open(&dest, fname, SFM_WRITE)) {
        goto FAIL;
    }

    if (sf_seek(src->file, start, SEEK_SET) < 0) {
        goto FAIL;
    }

    if (extract_section(src, &dest, start, end, songbuf, buflen) < 0) {
        goto FAIL;
    }

    audiofile_close(&dest);
    return 0;
FAIL:
    audiofile_close(&dest);
    return -1;
}
