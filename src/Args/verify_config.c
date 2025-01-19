#include <sys/stat.h>

#include "App/AppMode.h"
#include "App/app_menu.h"
#include "Args/ArgsConfig.h"
#include "Types/Error.h"
#include "Macro/assert.h"

static void
check_defaults(struct ArgsConfig *config, AppMode **interact) {
    if (!interact && !*interact) {
        *interact = &app_menu;
    }
}

static union Error
check_required(struct ArgsConfig *config, AppMode **interact) {
    DEBUG_ASSERT(interact && *interact != NULL, "missing callback");

    if (config->extract_dir) {
        struct stat dirstat;

        if (stat(config->extract_dir, &dirstat) != 0) {
            return error_msg("failed to stat file '%s'", config->extract_dir);
        }

        if (!S_ISDIR(dirstat.st_mode)) {
            return error_msg("not a directory: '%s'", config->extract_dir);
        }
    }

    return error_level(LEVEL_SUCCESS);
}

union Error
verify_config(struct ArgsConfig *config, AppMode **interact) {
    DEBUG_ASSERT(config != NULL, "null config");

    check_defaults(config, interact);
    return check_required(config, interact);
}
