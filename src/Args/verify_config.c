#include "Audio/interact_menu.h"
#include "Args/ArgsConfig.h"
#include "Types/Error.h"
#include "Debug/assert.h"

static void
check_defaults(struct ArgsConfig *config) {
    if (!config->interact) {
        config->interact = &interact_menu;
    }
}

static union Error
check_required(struct ArgsConfig *config) {
    (void) config;
    DEBUG_ASSERT(config->interact != NULL, "missing callback");

    return error_level(LEVEL_SUCCESS);
}

union Error
verify_config(struct ArgsConfig *config) {
    DEBUG_ASSERT(config != NULL, "null config");

    check_defaults(config);
    return check_required(config);
}
