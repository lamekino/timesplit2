#include "Args/ArgsXMacro.h"

#include "Macro/assert.h"

char
get_short_flag(ArgsXMacro xm) {
    static const char short_flags[] = {
#define YS(label, _1, short, ...) [label] = short,
        ARGUMENT_XMACRO(YS)
#undef YS
    };

    DEBUG_ASSERT(0 <= xm && xm < ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_short_flag");

    return short_flags[xm];
}

const char *
get_long_flag(ArgsXMacro xm) {
    static const char *long_flags[] = {
#define YS(label, _1, _2, long, ...) [label] = long,
        ARGUMENT_XMACRO(YS)
#undef YS
    };

    DEBUG_ASSERT(0 <= xm && xm < ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_long_flag");

    return long_flags[xm];
}

const char *
get_description(ArgsXMacro xm) {
    static const char *descriptions[] = {
#define YS(label, desc, ...) [label] = desc,
        ARGUMENT_XMACRO(YS)
#undef YS
    };

    DEBUG_ASSERT(0 <= xm && xm < ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_description");

    return descriptions[xm];
}

struct FlagInfo
get_flag_info(ArgsXMacro xm) {
    static const struct FlagInfo information[ARGUMENT_XMACRO_COUNT] = {
        [FLAG_EXTRACT_OUTPUT] = (struct FlagInfo) {
            .argcount = 1,
            .argtype = "dir"
        },

        [FLAG_TIMESTAMPS_FILE] = (struct FlagInfo) {
            .argcount = 1,
            .argtype = "filename"
        }
    };

    DEBUG_ASSERT(0 <= xm && xm < ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_description");

    return information[xm];
}
