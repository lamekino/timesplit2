#include <string.h>

#include "Args/process_args.h"
#include "Args/arguments_xmacro.h"
#include "Args/config.h"
#include "Args/usage.h"

#include "Debug/assert.h"

struct ShortFlag {
    char flag[3];
};

static struct ShortFlag
get_short_flag_string(ArgumentXMacro xm) {
    return (struct ShortFlag) {
        { '-', get_short_flag(xm), '\0' }
    };
}

static bool
is_flag(ArgumentXMacro xm, const char *arg, size_t arg_len) {
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
set_flag(ArgumentXMacro flag, struct ArgConfig *config) {
    DEBUG_ASSERT(config, "Null config in set_flag");

    int pending_args = 0;

    switch (flag) {
    case FLAG_HELP: {
        break;
    }
    case FLAG_EXTRACT_ALL: {
        config->use_extract_all = true;
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
set_flag_args(char **cur_arg[], int pending_args, ArgumentXMacro flag,
        struct ArgConfig *config) {
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

static ArgumentXMacro
resolve_argument_flag(const char *arg) {
    ArgumentXMacro xm;

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

static void
show_missing_args_error(const char *progname, ArgumentXMacro flag) {
    const char short_flag = get_short_flag(flag);
    const char *long_flag = get_long_flag(flag);

    fprintf(stderr, "Missing argument for: '-%c/%s'\n\n",
            short_flag, long_flag);

    /* TODO: encode # of arguments + argument type, ie:
     * <file1> <file2> ... <fileN> */
    fprintf(stderr, "Usage: %s [-%c|%s] ...",
            progname, short_flag, long_flag);
}

static void
show_unknown_error(const char *progname, const char *unknown) {
    fprintf(stderr, "Unknown flag: '%s'\n", unknown);
    usage(stderr, progname);
}

static void
set_help_flag(const char *progname, struct ArgConfig **cfg) {
    *cfg = NULL;
    usage(stdout, progname);
}

int
process_args(char *argv[], struct ArgConfig **cfg) {
    struct ArgConfig *config = *cfg;

    const char *progname = argv[0];

    char **argp = &argv[1];

    ArgumentXMacro flag = -1;
    int pending_args = -1;

    while (*argp) {
        char *cur = *argp;

        flag = resolve_argument_flag(cur);
        if (flag < 0) {
            if (config->audio_path != NULL) {
                show_unknown_error(progname, cur);
                return -1;
            }

            config->audio_path = cur;
            argp++;
            continue;
        }

        pending_args = set_flag(flag, *cfg);
        if (pending_args < 0) {
            fprintf(stderr, "Error in processing flag: '%s'", cur);
            return -1;
        }

        pending_args -= set_flag_args(&argp, pending_args, flag, *cfg);
        if (pending_args > 0) {
            show_missing_args_error(progname, flag);
            return -1;
        }
    }

    if (pending_args > 0) {
        show_missing_args_error(progname, flag);
        return -1;
    }

    if (flag == FLAG_HELP) {
        set_help_flag(progname, cfg);
        return -1;
    }

    return 0;
}
