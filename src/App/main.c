#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "Args/config.h"
#include "Args/process_args.h"
#include "Audio/audio_interact.h"
#include "Parser/parse_file.h"
#include "Parser/parse_stream.h"
#include "Types/Song.h"

/* TODO: factor out fprintfs into a function which can switch over an enum of
 * AppErrors, call fucntion app_error(enum AppError type) */
int
main(int argc, char *argv[]) {
    const char *audiopath = "./sample-audio/MzCEqlPp0L8.opus";
    const char *localename = "en_US.UTF-8";

    struct ArgConfig config_params = {0};
    struct ArgConfig *config = &config_params;

    struct Stack parsed = {0};

    /*
     * initialize song list
     */
    if (!stack_create(&parsed)) {
        fprintf(stderr, "could not create data stack\n");
        goto FAIL;
    }

    /*
     * set the locale to read utf-8
     */
    if (!setlocale(LC_ALL, localename)) {
        fprintf(stderr, "could not set locale to %s\n", localename);
        goto FAIL;
    }

    /*
     * process cli arguments
     */
    if (argc > 1 && process_args(argv, &config) < 0) {
        if (config == NULL) {
            goto SUCCESS;
        }

        fprintf(stderr, "could not process arguments\n");
        goto FAIL;
    }

    /*
     * parse timestamps
     */
    if (parse_file(config->timestamps_path, &parsed) < 0) {
        if (config->timestamps_path != NULL) {
            fprintf(stderr, "error in parsing\n");
            goto FAIL;
        }

        fprintf(stderr, "No timestamp file provided, paste them here:\n");
        parse_stream(stdin, &parsed);
    }

    /*
     * interact with audio file
     */
    if (audio_interact(audiopath, &parsed) < 0) {
        fprintf(stderr, "error in audio?? bad error\n");
        goto FAIL;
    }

    /*
     * finish
     */
SUCCESS:
    stack_cleanup(&parsed, free_song);
    return EXIT_SUCCESS;
FAIL:
    stack_cleanup(&parsed, free_song);
    return EXIT_FAILURE;
}
