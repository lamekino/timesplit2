#include <stdlib.h>

#include "App/app_init.h"
#include "Args/ArgsConfig.h"
#include "Args/process_args.h"
#include "Args/verify_config.h"
#include "Parser/parse_file.h"
#include "Types/Error.h"
#include "Types/Stack.h"
#include "Types/Song.h"
#include "Types/Error.h"

int
main(int argc, char *argv[]) {
    const char *locale = "en_US.UTF-8";

    union Error err = {0};
    struct Stack songs = {0};
    struct ArgsConfig config = {0};

    if (argc == 1) {
        err = error_msg("missing argument, use -h to view help");
        goto FAIL;
    }

    err = app_init(locale, argc, &songs);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    err = process_args(argv, &config);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    err = verify_config(&config);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    err = parse_file(config.timestamps_path, &songs);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    err = config.interact(config.extract_dir, config.audio_path, &songs);
    if (IS_ERROR(err)) {
        goto FAIL;
    }

    stack_cleanup(&songs, free_song);
    return EXIT_SUCCESS;

FAIL:
    stack_cleanup(&songs, free_song);
    return error_report(argv[0], &err);
}
