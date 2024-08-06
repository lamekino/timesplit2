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
short_flag_string(ArgumentXMacro xm) {
    return (struct ShortFlag) {
        { '-', get_short_flag(xm), '\0' }
    };
}

static int
is_flag(ArgumentXMacro xm, const char *arg, size_t arg_len) {
    struct ShortFlag sf = short_flag_string(xm);

    const char *short_flag = &sf.flag[0];
    const char *long_flag = get_long_flag(xm);

    return strncmp(arg, short_flag, arg_len) == 0
        || strncmp(arg, long_flag, arg_len) == 0;
}

static int
process_flag(ArgumentXMacro flag, struct ArgConfig **cfg) {
    DEBUG_ASSERT(cfg != NULL, "Null cfg in process_flag");
    DEBUG_ASSERT(*cfg != NULL, "Null config in process_flag");

    struct ArgConfig *const config = *cfg;

    switch (flag) {

    case FLAG_HELP:
        *cfg = NULL;
        break;

    case FLAG_EXTRACT_ALL:
        config->use_extract_all = true;
        break;

    default:
        DEBUG_ASSERT(0, "Missing impl for process_flag");
        return -1;

    }

    return 0;

}

static int
process_arg(const char *arg, struct ArgConfig **cfg) {
    ArgumentXMacro flag;

    const size_t arglen = strlen(arg);
    struct ArgConfig *const config = *cfg;

    if (arglen <= 1 || config == NULL) {
        return -1;
    }

    for (flag = 0; flag < ARGUMENT_XMACRO_COUNT; flag++) {
        bool has_flag = is_flag(flag, arg, arglen);

        if (has_flag) {
            return process_flag(flag, cfg);
        }
    }

    return -1;
}

int
process_args(char *argv[], struct ArgConfig **cfg) {
    char **cur = NULL;

    for (cur = &argv[1]; *cur; cur++) {
        if (process_arg(*cur, cfg) < 0) {
            fprintf(stderr, "Unknown flag: '%s'\n", *cur);
            usage(stderr, argv[0]);
            return -1;
        }
    }

    if (*cfg == NULL) {
        usage(stdout, argv[0]);
        return -1;
    }

    return 0;
}
