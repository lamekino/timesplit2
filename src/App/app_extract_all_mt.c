#include <sndfile.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>

#include "App/AppOutput.h"
#include "App/app_extract_all_mt.h"
#include "Args/ArgsConfig.h"
#include "Audio/AudioFile.h"
#include "Audio/extract_song.h"
#include "Audio/song_frame_offset.h"
#include "Macro/min.h"
#include "Types/Error.h"
#include "Types/Song.h"
#include "Types/Stack.h"

#define BUFFER_SIZE 4096u
/* TODO: use dynamic allocation, remove max */
#define THREAD_MAX 16
#define DEFAULT_THREAD_COUNT 4

#define THREAD_ERROR(err) ((void *) (err).description)

typedef double sndbuffer_t;

struct ThreadParams {
    const struct Stack *tracklist;

    const char *outdir;
    const char *srcfile;

    size_t *shared_idx;
    sndbuffer_t *sndbuffer;
    pthread_mutex_t *lock;
};

static void *
app_extract_worker(void *thread_params) {
    union Error y = {0};

    struct ThreadParams *thread = (struct ThreadParams *) thread_params;

    const char *srcfile = thread->srcfile;
    const char *outdir = thread->outdir;

    sndbuffer_t *sndbuffer = thread->sndbuffer;
    const struct Stack *tracks = thread->tracklist;

    struct Song *cursong = NULL;
    struct Song *nextsong = NULL;

    struct AudioFile audiosrc = {0};

    if (!audiofile_open(&audiosrc, srcfile, SFM_READ)) {
        y = error_msg("could not open '%s': %s",
                srcfile, sf_error(audiosrc.file));
        goto THREAD_EXIT;
    }

    while (*thread->shared_idx < tracks->count) {
        struct AppOutput output;

        if (pthread_mutex_lock(thread->lock) != 0) {
            y = error_msg("could not lock thread: %s", strerror(errno));
            goto THREAD_EXIT;
        }

        cursong = stack_mod_index(tracks, *thread->shared_idx);
        (*thread->shared_idx)++;
        nextsong = stack_mod_index(tracks, *thread->shared_idx);

        if (pthread_mutex_unlock(thread->lock) != 0) {
            y = error_msg("could not unlock thread: %s", strerror(errno));
            goto THREAD_EXIT;
        }

        output = app_output_create(outdir, cursong, nextsong, &audiosrc);

        fprintf(stderr, "Extracting: '%ls' [+%ld]\n",
                cursong->title, cursong->timestamp);

        if (extract_song(&audiosrc, &output, sndbuffer, BUFFER_SIZE) < 0) {
            fprintf(stderr, "Failed to extract '%ls'!\n", cursong->title);
        }
    }

THREAD_EXIT:
    if (audiofile_close(&audiosrc) != 0 && !IS_ERROR(y)) {
        y = error_msg("couldn't close audio source: %s", strerror(errno));
    }

    return THREAD_ERROR(y);
}

union Error
app_extract_all_mt(const struct ArgsConfig *config, const struct Stack *ts) {
    union Error y = {0};

    size_t ti;

    pthread_t threads[THREAD_MAX];

    sndbuffer_t thread_buffers[THREAD_MAX][BUFFER_SIZE];

    struct ThreadParams thread_params[THREAD_MAX];

    uintptr_t *thread_rv = NULL;

    size_t shared_idx = 0;

    pthread_mutex_t thread_mutex;

    size_t use_threads = MIN(ts->count, config->thread_count);

    if (use_threads == 0) {
        use_threads = MIN(ts->count, DEFAULT_THREAD_COUNT);
    }

    pthread_mutex_init(&thread_mutex, NULL);

    for (ti = 0; ti < use_threads; ti++) {
        void *vp = (void *) &thread_params[ti];

        thread_params[ti] = (struct ThreadParams) {
            .lock = &thread_mutex,
            .sndbuffer = thread_buffers[ti],
            .shared_idx = &shared_idx,
            .tracklist = ts,
            .outdir = config->extract_dir,
            .srcfile = config->audio_path
        };

        if (pthread_create(&threads[ti], NULL, &app_extract_worker, vp) != 0) {
            y = error_msg("could not create thread: %s",
                    strerror(errno));
            goto APP_FAIL;
        }
    }

    for (ti = 0; ti < use_threads; ti++) {
        if (pthread_join(threads[ti], (void *) &thread_rv) != 0) {
            y = error_msg("could not join thread: %s",
                    strerror(errno));
            goto APP_FAIL;
        }

        if (thread_rv != NULL) {
            y = (union Error) { .description = (char *) thread_rv };
            goto APP_FAIL;
        }
    }

APP_FAIL:
    if (IS_ERROR(y)) {
        return y;
    }

    return error_level(LEVEL_SUCCESS);
}
