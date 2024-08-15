#include <stdbool.h>
#include <string.h>

#include "Args/process_args.h"
#include "Args/ArgsXMacro.h"
#include "Args/ArgsConfig.h"
#include "Args/usage.h"

#include "App/app_extract_all.h"
#include "Debug/assert.h"
#include "Types/Error.h"

struct ShortFlag {
    char flag[3];
};

static struct ShortFlag
get_short_flag_string(ArgsXMacro xm) {
    return (struct ShortFlag) {
        { '-', get_short_flag(xm), '\0' }
    };
}

static bool
is_flag(ArgsXMacro xm, const char *arg, size_t arg_len) {
    struct ShortFlag sf = get_short_flag_string(xm);

    const char *short_flag = &sf.flag[0];
    const char *long_flag = get_long_flag(xm);

    if (arg_len < 2) {
        return false;
    }

    return strncmp(arg, short_flag, arg_len) == 0
        || strncmp(arg, long_flag, arg_len) == 0;
}

/* TODO: set_flag.c with all the setter functions (set help, set_extract_all,
 * etc) */
static int
set_flag(ArgsXMacro flag, struct ArgsConfig *config) {
    DEBUG_ASSERT(config, "Null config in set_flag");

    int pending_args = 0;

    switch (flag) {
    case FLAG_HELP: {
        break;
    }
    case FLAG_EXTRACT_ALL: {
        config->interact = &app_extract_all;
        break;
    }
    case FLAG_EXTRACT_OUTPUT: {
        pending_args = 1;
        break;
    }
    case FLAG_TIMESTAMPS_FILE: {
        pending_args = 1;
        break;
    }
    default: {
        DEBUG_ASSERT(0, "Missing impl for process_flag");
        return -1;
    }}

    return pending_args;
}

static int
set_next_arg(char **cur_arg, char **field) {
    char *next_arg = *(cur_arg + 1);

    if (next_arg == NULL) {
        return -1;
    }

    *field = next_arg;
    return 0;
}

static int
set_flag_args(char **cur_arg[], int pending_args, ArgsXMacro flag,
        struct ArgsConfig *config) {
    DEBUG_ASSERT(cur_arg, "next_arg: cur_arg null");
    DEBUG_ASSERT(*cur_arg, "next_arg: cur_arg null");
    DEBUG_ASSERT(**cur_arg, "next_arg: **cur_arg null");

    const size_t seek = pending_args + 1;

    if (cur_arg == NULL || *cur_arg == NULL || **cur_arg == NULL) {
        return -1;
    }

    if (pending_args == 0) {
        goto FINISH;
    }

    switch (flag) {
    /* TODO: encode pending args w/ flag somehow so we don't have to couple two
     * function's behaviors */
    case FLAG_EXTRACT_OUTPUT: {
        DEBUG_ASSERT(pending_args == 1, "next_arg: pending_args mismatch");

        if (set_next_arg(*cur_arg, &config->extract_dir) < 0) {
            return -1;
        }
        break;
    }
    case FLAG_TIMESTAMPS_FILE: {
        DEBUG_ASSERT(pending_args == 1, "next_arg: pending_args mismatch");

        if (set_next_arg(*cur_arg, &config->timestamps_path) < 0) {
            return -1;
        }
        break;
    }
    default: {
        DEBUG_ASSERT(0, "Missing impl for next_arg");
        return -1;
    }}

FINISH:
    *cur_arg += seek;
    return seek;
}

static ArgsXMacro
resolve_argument_flag(const char *arg) {
    ArgsXMacro xm;

    const size_t arglen = strlen(arg);

    if (arglen <= 1) {
        return -1;
    }

    for (xm = 0; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        bool has_flag = is_flag(xm, arg, arglen);

        if (has_flag) {
            return xm;
        }
    }

    return -1;
}

union Error
process_args(char *argv[], struct ArgsConfig **cfg) {
    struct ArgsConfig *config = *cfg;

    const char *progname = argv[0];

    char **argp = &argv[1];

    ArgsXMacro flag = -1;
    int pending_args = -1;

    while (*argp) {
        char *cur = *argp;

        flag = resolve_argument_flag(cur);
        if (flag < 0 && config->audio_path != NULL) {
            return error_msg("Unknown flag: '%s'\n", cur);
        }

        if (flag < 0) {
            config->audio_path = cur;
            argp++;
            continue;
        }

        pending_args = set_flag(flag, *cfg);
        if (pending_args < 0) {
            return error_msg("Error in processing flag: '%s'", cur);
        }

        pending_args -= set_flag_args(&argp, pending_args, flag, *cfg);
        if (pending_args > 0) {
            return error_msg("Missing argument for: '%s'", cur);
        }
    }

    if (pending_args > 0) {
        return error_msg("Missing argument for: '%s'", argp[-1]);
    }

    if (flag == FLAG_HELP) {
        return error_level(LEVEL_SHOW_HELP);
    }

    return error_level(LEVEL_SUCCESS);
}
