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
#include "Macro/max.h"
#include "Types/Error.h"
#include "Types/Song.h"
#include "Types/Stack.h"

#define BUFFER_SIZE 4096u
/* TODO: use dynamic allocation, remove max */
#define THREAD_MAX 16

typedef double sndbuffer_t;

typedef sndbuffer_t ThreadBuffer[BUFFER_SIZE];

struct ThreadParam {
    const struct Stack *tracklist;

    const char *outdir;
    const char *srcfile;
    sndbuffer_t *sndbuffer;

    size_t *shared_idx;
    pthread_mutex_t *shared_lock;
};

static size_t
app_thread_count(size_t request, size_t item_count) {
    const size_t needed = MIN(request, item_count);
    const size_t capacity = THREAD_MAX;
    const size_t minimum = 1;

    return MIN(capacity, MAX(minimum, needed));
}

static int
app_extract_to_output(const char *outdir, AudioFile *src,
        Song *cur, Song *next, ThreadBuffer sndbuffer) {
    struct AppOutput out = app_output_create(outdir, cur, next, src);

    fprintf(stderr, "Extracting: '%ls' [+%ld]\n", cur->title, cur->timestamp);

    if (extract_song(src, &out, sndbuffer, BUFFER_SIZE) < 0) {
        fprintf(stderr, "Failed to extract '%ls'!\n", cur->title);
        return -1;
    }

    return 0;
}

static void *
app_extract_worker(void *thread_param) {
    union Error y = {0};

    struct ThreadParam *thread = (struct ThreadParam *) thread_param;

    const char *srcfile = thread->srcfile;
    const char *outdir = thread->outdir;

    sndbuffer_t *sndbuffer = thread->sndbuffer;
    const struct Stack *tracks = thread->tracklist;

    struct Song *cur = NULL;
    struct Song *next = NULL;

    struct AudioFile audio = {0};

    if (!audiofile_open(&audio, srcfile, SFM_READ)) {
        y = error_msg("could not open '%s': %s", srcfile, sf_error(audio.file));
        goto THREAD_EXIT;
    }

    while (*thread->shared_idx < tracks->count) {
        /* critical section enter */
        if (pthread_mutex_lock(thread->shared_lock) != 0) {
            y = error_msg("could not lock thread: %s", strerror(errno));
            goto THREAD_EXIT;
        }

        /* critical section work */
        cur = stack_mod_index(tracks, *thread->shared_idx);
        (*thread->shared_idx)++;
        next = stack_mod_index(tracks, *thread->shared_idx);

        /* critical section exit */
        if (pthread_mutex_unlock(thread->shared_lock) != 0) {
            y = error_msg("could not unlock thread: %s", strerror(errno));
            goto THREAD_EXIT;
        }

        if (app_extract_to_output(outdir, &audio, cur, next, sndbuffer) < 0) {
            continue;
        }
    }

THREAD_EXIT:
    if (audiofile_close(&audio) != 0 && !IS_ERROR(y)) {
        y = error_msg("couldn't close audio source: %s", strerror(errno));
    }

    return y.description;
}

union Error
app_extract_all_mt(const struct ArgsConfig *config, const struct Stack *ts) {
    size_t t;
    size_t thread_count = app_thread_count(config->thread_count, ts->count);

    pthread_t threads[THREAD_MAX];
    ThreadBuffer thread_buffers[THREAD_MAX];
    struct ThreadParam thread_params[THREAD_MAX];

    void *thread_rv = NULL;

    size_t shared_idx = 0;
    pthread_mutex_t shared_mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_init(&shared_mutex, NULL);

    for (t = 0; t < thread_count; t++) {
        void *vp = (void *) &thread_params[t];

        thread_params[t] = (struct ThreadParam) {
            .shared_lock = &shared_mutex,
            .shared_idx = &shared_idx,
            .tracklist = ts,

            .sndbuffer = thread_buffers[t],
            .outdir = config->extract_dir,
            .srcfile = config->audio_path,
        };

        if (pthread_create(&threads[t], NULL, &app_extract_worker, vp) != 0) {
            return error_msg("could not create thread: %s", strerror(errno));
        }
    }

    for (t = 0; t < thread_count; t++) {
        if (pthread_join(threads[t], &thread_rv) != 0) {
            return error_msg("could not join thread: %s", strerror(errno));
        }

        if (thread_rv != NULL) {
            return (union Error) { .description = thread_rv };
        }
    }

    return error_level(LEVEL_SUCCESS);
}
