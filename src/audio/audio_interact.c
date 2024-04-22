#include <ctype.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#include "types/stack.h"
#include "types/song.h"

#define DEFAULT_PROMPT "type -1 to quit >> "
#define MSG_PROMPT(msg) (msg "\n" DEFAULT_PROMPT)
#define FRAME_END -2

#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define LENGTH(xs) (sizeof(xs)/sizeof((xs)[0]))

struct AudioFile {
    SNDFILE *file;
    SF_INFO info;
};

/* TODO: I like this, move to appropiate spots */
typedef const struct AudioFile AudioFile;
typedef const struct Song Song;
typedef const struct Stack Parsed;

#define AUDIO_OPEN(af, path, mode) \
    ( (af).file = sf_open((path), (mode), &(af).info), \
      (af).file \
      )

#define AUDIO_CLOSE(af) \
    sf_close((af).file)

union UserIndex {
    int got;
    size_t idx;
};

static union UserIndex
read_int() {
    char buf[512] = {0};

    if (!fgets(buf, sizeof(buf), stdin)) {
        return (union UserIndex) {-1};
    }

    return (union UserIndex) {atoi(buf)};
}

static bool
read_yes_no(const char *fmt, ...) {
    char got[4];
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, " [Y/n] ");
    va_end(ap);

    if (!fgets(got, sizeof(got), stdin)) {
        return false;
    }


    return tolower(got[0]) == 'y';
}

static union UserIndex
menu_interact(Parsed *parsed, const char *fmt, ...) {
    size_t i;

    for (i = 0; i < parsed->count; i++) {
        const struct Song *cur = parsed->elems[i];

        printf("[%zu] %ls [+%lds]\n", i + 1, cur->title, cur->timestamp);
    }

    {
        va_list ap;

        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }

    return read_int();
}

/* TODO:
 * move to types/parsed.h
 * make stack impl generic (macros :P) and define implementation here
 * rename struct Stack to struct SongStack
 * typedef const struct SongStack Parsed;
 * circular_index(Parsed *parsed, size_t idx);
 * include other parser specific stack operations here
 */
static struct Song *
circular_index(Parsed *parsed, size_t idx) {
    return parsed->elems[idx % parsed->count];
}

static sf_count_t
frame_offset(time_t time, int samplerate) {
    return time * samplerate;
}

static sf_count_t
song_frame_offset(Song *song, int samplerate) {
    return frame_offset(song->timestamp, samplerate);
}

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

/* TODO:
 * instead of taking the total_copy_frames, take the next timestamp and copy
 * while (copy_amount + start < frames(next)) */
static int
song_extract(AudioFile *src, Song *song, sf_count_t position, sf_count_t ending,
        double *songbuf, sf_count_t buflen) {
    const int channels = src->info.channels;
    const int format = src->info.format;

    char filename[512] = {0};
    SF_INFO outinfo = src->info;
    SNDFILE *outfile = NULL;


    song_filename(song, format, filename, sizeof(filename));
    outfile = sf_open(filename, SFM_WRITE, &outinfo);
    if (!outfile) {
        return -1;
    }

    while (position < ending) {
        sf_count_t seeked = (ending - position) * channels;
        sf_count_t copy_size = MIN(seeked, buflen);

        if (sf_read_double(src->file, songbuf, copy_size) <= 0) {
            goto BAIL;
        }

        if (sf_write_double(outfile, songbuf, copy_size) != copy_size) {
            goto BAIL;
        }

        position += copy_size / channels;
    }

    sf_close(outfile);
    return 0;
BAIL:
    sf_close(outfile);
    return -1;
}

/* this approach should make extracting all songs from the file trival
 *
 * ie,
 * #define next(idx, c) ((idx) + 1) % c)
 * for (idx = 0; idx < parsed->count; idx++) {
 *      cur = parsed->elems[idx];
 *      next = parsed->elems[next(idx, parsed->count)];
 *
 *      if (next->timestamp == 0) read_until_end(sndfile, cur)
 *      else read_until read_until(sndfile, cur, next);
 * }
 */
static int
song_interact(AudioFile *src, Parsed *parsed, size_t idx,
        double *songbuf, sf_count_t buflen) {
    const int rate = src->info.samplerate;

    const struct Song *cur = circular_index(parsed, idx);
    const struct Song *next = circular_index(parsed, idx + 1);

    const sf_count_t start = song_frame_offset(cur, rate);
    const sf_count_t finish = song_frame_offset(next, rate);

    if (!src->info.seekable) {
        return -1;
    }

    if (!read_yes_no("extract '%ls'?", cur->title)) {
        return -1;
    }

    if (sf_seek(src->file, start, SEEK_SET) < 0) {
        return -1;
    }

    if (song_extract(src, cur, start, finish, songbuf, buflen) < 0) {
        return -1;
    }

    {
        fprintf(stderr,"done! [Enter]");
        read_int();
        return 0;
    }

    return 0;
}

int
audio_interact(const char *audiopath, const struct Stack *parsed) {
    struct AudioFile audio = {0};

    double songbuf[4096];
    const size_t len = LENGTH(songbuf);

    if (!AUDIO_OPEN(audio, audiopath, SFM_READ)) {
        return -1;
    }

    while (1) {
        union UserIndex in = menu_interact(parsed, DEFAULT_PROMPT);

        if (in.got == EOF) {
            break;
        }

        /* if the recieved is negative, then it must be bigger than a realisic
         * parsed->count size in unsigned binary */
        while (!(1 <= in.idx && in.idx <= parsed->count)) {
            in = menu_interact(parsed, MSG_PROMPT("invalid value: %d"), in.got);
            if (in.got == EOF) {
                break;
            }
        }

        if (song_interact(&audio, parsed, in.idx - 1, songbuf, len) < 0) {
            break;
        }
    }

    AUDIO_CLOSE(audio);
    return 0;
}