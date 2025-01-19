#include <sndfile.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "App/AppOutput.h"
#include "Audio/AudioFile.h"
#include "Audio/extract_song.h"
#include "Audio/song_frame_offset.h"
#include "Types/Error.h"
#include "App/app_extract_all_mt.h"
#include "Types/Song.h"
#include "Types/Stack.h"
#include "Macro/min.h"

#define BUFFER_SIZE 4096u
#define THREAD_COUNT 13

#define THREAD_ERROR(err) ((void *) (err).description)

typedef double sndbuffer_t;

struct ThreadParams {
    const struct Stack *remaining;

    const char *outdir;
    const char *srcfile;

    sndbuffer_t *sndbuffer;
    pthread_mutex_t *lock;
};

static void *
app_extract_worker(void *extract_params) {
    static size_t counter = 0;

    struct ThreadParams *thread = (struct ThreadParams *) extract_params;

    const char *srcfile = thread->srcfile;
    const char *outdir = thread->outdir;

    sndbuffer_t *sndbuffer = thread->sndbuffer;
    const struct Stack *tracks = thread->remaining;

    struct Song *cursong = NULL;
    struct Song *nextsong = NULL;

    struct AudioFile audiosrc = {0};

    union Error err = {0};

    if (!audiofile_open(&audiosrc, srcfile, SFM_READ)) {
        err = error_msg("could not open '%s', %s",
                srcfile, sf_error(audiosrc.file));
        goto FAIL;
    }

    while (counter < tracks->count) {
        struct AppOutput output;

        if (pthread_mutex_lock(thread->lock) != 0) {
            err = error_msg("could not lock thread: %s", strerror(errno));
            goto FAIL;
        }

        /* TODO: use a queue instead */
        cursong = stack_mod_index(tracks, counter);
        counter++;
        nextsong = stack_mod_index(tracks, counter);

        if (pthread_mutex_unlock(thread->lock) != 0) {
            err = error_msg("could not unlock thread: %s", strerror(errno));
            goto FAIL;
        }

        output = app_output_create(outdir, cursong, nextsong, &audiosrc);

        fprintf(stderr, "Extracting: '%ls' [+%ld]\n",
                cursong->title, cursong->timestamp);

        if (extract_song(&audiosrc, &output, sndbuffer, BUFFER_SIZE) < 0) {
            fprintf(stderr, "Failed to extract '%ls'!\n", cursong->title);
            continue;
        }
    }

    if (audiofile_close(&audiosrc) != 0) {
        err = error_msg("couldn't close audio source: %s", strerror(errno));
        goto FAIL;
    }

    return NULL;
FAIL:
    return THREAD_ERROR(err);
}

union Error
app_extract_all_mt(const char *outdir, const char *audiopath,
        const struct Stack *ts) {
    size_t idx;

    size_t use_threads = MIN(ts->count, THREAD_COUNT);

    pthread_t threads[THREAD_COUNT];

    sndbuffer_t thread_buffers[THREAD_COUNT][BUFFER_SIZE];

    struct ThreadParams thread_params[THREAD_COUNT];

    pthread_mutex_t thread_mutex;

    pthread_mutex_init(&thread_mutex, NULL);

    for (idx = 0; idx < use_threads; idx++) {
        void *vp = (void *) &thread_params[idx];

        thread_params[idx] = (struct ThreadParams) {
            .lock = &thread_mutex,
            .sndbuffer = thread_buffers[idx],
            .remaining = ts,
            .outdir = outdir,
            .srcfile = audiopath
        };

        if (pthread_create(&threads[idx], NULL, &app_extract_worker, vp) != 0) {
            return error_msg("could not create thread, exit with: %s",
                    strerror(errno));
        }
    }

    for (idx = 0; idx < use_threads; idx++) {
        uintptr_t *rv = NULL;
        if (pthread_join(threads[idx], (void *) &rv) != 0) {
            continue; /* ??? */
        }

        if (rv != NULL) {
            return (union Error) { *rv };
        }
    }

    return error_level(LEVEL_SUCCESS);
}
