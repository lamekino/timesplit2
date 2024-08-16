#include <ctype.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Types/Error.h"
#include "Audio/extract_song.h"
#include "Audio/soundfile.h"
#include "Debug/assert.h"
#include "Audio/song_frame_offset.h"
#include "App/app_menu.h"

#include "Types/Stack.h"

#define DEFAULT_PROMPT "type -1 to quit >> "
#define MSG_PROMPT(msg) (msg "\n" DEFAULT_PROMPT)

#define LENGTH(xs) (sizeof(xs)/sizeof((xs)[0]))

typedef const struct Stack Timestamps;

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
menu_interact(Timestamps *ts, const char *fmt, ...) {
    size_t i;

    for (i = 0; i < ts->count; i++) {
        Song *cur = ts->elems[i];

        printf("[%zu] \"%ls\" [+%lds]\n", i + 1, cur->title, cur->timestamp);
    }

    {
        va_list ap;

        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }

    return read_int();
}

static int
song_interact(SoundFile *src, Timestamps *ts, size_t idx,
        double *songbuf, sf_count_t buflen) {
    const int rate = src->info.samplerate;

    Song *cur = stack_mod_index(ts, idx);
    Song *next = stack_mod_index(ts, idx + 1);

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

    if (extract_song(src, cur, start, finish, songbuf, buflen) < 0) {
        return -1;
    }

    {
        fprintf(stderr,"done! [Enter]");
        read_int();
        return 0;
    }

    return 0;
}

union Error
app_menu(const char *audiopath, Timestamps *ts) {
    struct SoundFile snd = {0};

    double songbuf[4096];
    const size_t len = LENGTH(songbuf);

    if (audiopath == NULL) {
        return error_msg("no audio provided");
    }

    if (!soundfile_open(&snd, audiopath, SFM_READ)) {
        return error_msg("could not open '%s': %s",
                audiopath, sf_strerror(snd.file));
    }

    while (true) {
        union UserIndex in = menu_interact(ts, DEFAULT_PROMPT);

        if (in.got == EOF) {
            break;
        }

        /* if the recieved is negative, then it must be bigger than a realisic
         * parsed->count size in unsigned binary */
        while (!(1 <= in.idx && in.idx <= ts->count)) {
            in = menu_interact(ts, MSG_PROMPT("invalid value: %d"), in.got);
            if (in.got == EOF) {
                break;
            }
        }

        /* FIXME: handle this error */
        if (song_interact(&snd, ts, in.idx - 1, songbuf, len) < 0) {
            break;
        }
    }

    soundfile_close(&snd);
    return error_level(LEVEL_SUCCESS);
}
