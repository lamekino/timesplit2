#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "Args/ArgsConfig.h"
#include "Args/process_args.h"
#include "Args/usage.h"
#include "Args/verify_config.h"
#include "Audio/interact_menu.h"
#include "Debug/assert.h"
#include "Parser/parse_file.h"
#include "Types/Error.h"
#include "Types/Song.h"

int
main(int argc, char *argv[]) {
    const char *localename = "en_US.UTF-8";

    union Error err = {0};
    struct Stack parsed = {0};


    /* TODO: make this part of process_args */
    struct ArgsConfig params = { .interact = &interact_menu };
    struct ArgsConfig *config = &params;

    /* check argc */
    if (argc < 2) {
        usage(stderr, argv[0]);
        err = error_level(LEVEL_FAILED);
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
    else if (IS_ERROR_LEVEL(err, LEVEL_SHOW_HELP)) {
        goto SHOW_USAGE;
    }

    /* verify config and set defaults */
    err = verify_config(config);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    /* parse timestamps */
    err = parse_file(config->timestamps_path, &parsed);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    /* interact with audio */
    err = config->interact(config->audio_path, &parsed);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    /* finish */
    stack_cleanup(&parsed, free_song);
    return EXIT_SUCCESS;
SHOW_USAGE:
    usage(stdout, argv[0]);
FAIL:
    stack_cleanup(&parsed, free_song);
    return error_report(&err);
}
