#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "Args/ArgsConfig.h"
#include "Args/process_args.h"
#include "Args/usage.h"
#include "Audio/audio_interact.h"
#include "Parser/parse_file.h"
#include "Parser/parse_stream.h"
#include "Types/Song.h"
#include "Types/Error.h"

/* TODO: factor out fprintfs into a function which can switch over an enum of
 * AppErrors, call fucntion app_error(enum AppError type) */
int
main(int argc, char *argv[]) {
    const char *localename = "en_US.UTF-8";

    union Error err = {0};
    struct Stack parsed = {0};

    struct ArgsConfig params = {0};
    struct ArgsConfig *config = &params;

    /* check argc */
    if (argc < 2) {
        usage(stderr, argv[0]);
        goto FAIL;
    }

    /* initialize song list */
    if (!stack_create(&parsed)) {
        err = error_level(LEVEL_NO_MEM);
        goto FAIL;
    }

    /* set the locale to read utf-8 */
    if (!setlocale(LC_ALL, localename)) {
        err = error_msg("could not set locale to '%s'\n", localename);
        goto FAIL;
    }

    /* process cli arguments */
    err = process_args(argv, &config);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    if (config == NULL) {
        goto SUCCESS;
    }

    /* parse timestamps */
    err = parse_file(config->timestamps_path, &parsed);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    /* interact with audio file */
    err = audio_interact(config->audio_path, &parsed);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    /* finish */
SUCCESS:
    stack_cleanup(&parsed, free_song);
    return EXIT_SUCCESS;
FAIL:
    stack_cleanup(&parsed, free_song);
    return error_report(&err);
}
