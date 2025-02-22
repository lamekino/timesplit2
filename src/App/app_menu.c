#include <ctype.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "App/AppOutput.h"
#include "Args/ArgsConfig.h"
#include "Audio/AudioFile.h"
#include "Types/Error.h"
#include "Audio/extract_song.h"
#include "Audio/AudioFile.h"
#include "Macro/assert.h"
#include "Macro/length.h"
#include "Audio/song_frame_offset.h"
#include "App/app_menu.h"

#include "Types/Stack.h"

#define DEFAULT_PROMPT "type -1 to quit >> "
#define MSG_PROMPT(msg) (msg "\n" DEFAULT_PROMPT)

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
menu_interact(const struct Stack *ts, const char *fmt, ...) {
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

enum SongInteractFail {
    SONG_INTERACT_OK,
    SONG_INTERACT_FAIL_SEEKABLE,
    SONG_INTERACT_FAIL_PROMPT,
    SONG_INTERACT_FAIL_EXTRACT
};

static enum SongInteractFail
song_interact(const char *outdir, AudioFile *src, const struct Stack *ts,
        size_t idx, double *songbuf, sf_count_t buflen) {
    Song *cur = stack_mod_index(ts, idx);
    Song *next = stack_mod_index(ts, idx + 1);

    AppOutput out = app_output_create(outdir, cur, next, src);

    if (!src->info.seekable) {
        return SONG_INTERACT_FAIL_SEEKABLE;
    }

    if (!read_yes_no("extract '%ls'?", cur->title)) {
        return SONG_INTERACT_FAIL_PROMPT;
    }

    if (extract_song(src, &out, songbuf, buflen) < 0) {
        return SONG_INTERACT_FAIL_EXTRACT;
    }

    fprintf(stderr,"done! [Enter]");
    read_int();
    return SONG_INTERACT_OK;
}

union Error
app_menu(const struct ArgsConfig *config, const struct Stack *ts) {
    union Error y = error_level(LEVEL_SUCCESS);

    const char *outdir = config->extract_dir;

    struct AudioFile audio = {0};

    double songbuf[4096];
    const size_t len = LENGTH(songbuf);

    union UserIndex in;

    if (config->audio_path == NULL) {
        return error_msg("no audio provided");
    }

    if (!audiofile_open(&audio, config->audio_path, SFM_READ)) {
        return error_msg("could not open '%s': %s",
                config->audio_path, sf_strerror(audio.file));
    }

    for (
        in = menu_interact(ts, DEFAULT_PROMPT);
        !IS_ERROR(y) && in.got != EOF;
        in = menu_interact(ts, DEFAULT_PROMPT)
    ) {
        /* if the recieved is negative, then it must be bigger than a realisic
         * parsed->count size in unsigned binary */
        if (!(1 <= in.idx && in.idx <= ts->count)) {
            in = menu_interact(ts, MSG_PROMPT("invalid value: %d"), in.got);
            continue;
        }

        switch (song_interact(outdir, &audio, ts, in.idx - 1, songbuf, len)) {
        case SONG_INTERACT_OK: {
            break;
        }
        case SONG_INTERACT_FAIL_PROMPT: {
            y = error_level(LEVEL_FAILED);
            break;
        }
        case SONG_INTERACT_FAIL_SEEKABLE: {
            y = error_msg("file has no seek: '%s'", audio.file);
            break;
        }
        case SONG_INTERACT_FAIL_EXTRACT: {
            y = error_msg("failed to extract: '%s'", audio.file);
            break;
        }
        default: {
            DEBUG_ASSERT(0, "unreachable");

            y = error_level(LEVEL_FAILED);
            break;
        }}
    }

    audiofile_close(&audio);
    return y;
}
