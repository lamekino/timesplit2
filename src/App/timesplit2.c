#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "Args/Args.h"
#include "Audio/Audio.h"
#include "Parser/Parser.h"
#include "Parser/parse_stream.h"
#include "Song/Song.h"

/* TODO: factor out fprintfs into a function which can switch over an enum of
 * AppErrors, call fucntion app_error(enum AppError type) */
int
timesplit2(int argc, char *argv[]) {
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

#if 0
#define p(x) #x, ((x)? (x) : "unset")
    {
        printf("%-40s %s\n", p(config->extract_dir));
        printf("%-40s %s\n", p(config->audio_path));
        printf("%-40s %s\n", p(config->timestamps_path));
        goto SUCCESS;
    }
#endif

    if (parse_file(config->timestamps_path, &parsed) < 0) {
        if (config->timestamps_path != NULL) {
            fprintf(stderr, "error in parsing\n");
            goto FAIL;
        }

        fprintf(stderr, "No timestamp file provided, paste them here:\n");
        parse_stream(stdin, &parsed);
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
