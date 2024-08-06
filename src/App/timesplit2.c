#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "Args/Args.h"
#include "Audio/Audio.h"
#include "Parser/Parser.h"
#include "Song/Song.h"

/* TODO: factor out fprintfs into a function which can switch over an enum of
 * AppErrors, call fucntion app_error(enum AppError type) */
int
timesplit2(int argc, char *argv[]) {
    const char *filename = "./sample-input/MzCEqlPp0L8";
    const char *audiopath = "./sample-audio/MzCEqlPp0L8.opus";
    const char *localename = "en_US.UTF-8";

    struct ArgConfig config_params = {0};
    struct ArgConfig *config = &config_params;

    struct SongList parsed = {0};

    if (!songlist_stack_create(&parsed)) {
        fprintf(stderr, "could not create data stack\n");
        goto FAIL;
    }

    if (!setlocale(LC_ALL, localename)) {
        fprintf(stderr, "could not set locale to %s\n", localename);
        goto FAIL;
    }

    if (argc > 1 && process_args(argv, &config) < 0) {
        if (config == NULL) {
            goto SUCCESS;
        }

        fprintf(stderr, "could not process arguments\n");
        goto FAIL;
    }

    if (parse_file(filename, &parsed) < 0) {
        fprintf(stderr, "error in parsing\n");
        goto FAIL;
    }

    if (audio_interact(audiopath, &parsed) < 0) {
        fprintf(stderr, "error in audio?? bad error\n");
        goto FAIL;
    }

SUCCESS:
    songlist_cleanup(&parsed, free_song);
    return EXIT_SUCCESS;
FAIL:
    songlist_cleanup(&parsed, free_song);
    return EXIT_FAILURE;
}
