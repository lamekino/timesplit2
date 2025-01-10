#include <limits.h>
#include <sndfile.h>

#include "App/AppOutput.h"
#include "Types/Song.h"
#include "Audio/AudioFile.h"
#include "Audio/extract_song.h"
#include "Macro/assert.h"
#include "Macro/min.h"

static int
extract_section(AudioFile *src, AudioFile *dest, const sf_count_t start,
        const sf_count_t finish, double *songbuf, sf_count_t buflen) {
    const int channels = src->info.channels;

    sf_count_t pos, copy_size;

    for (
        pos = start;
        pos < finish;
        pos += copy_size / channels
    ) {
        sf_count_t remaining = (finish - pos) * channels;

        copy_size = MIN(remaining, buflen);

        if (sf_read_double(src->file, songbuf, copy_size) <= 0) {
            return -1;
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
