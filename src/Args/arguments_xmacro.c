#include "Args/arguments_xmacro.h"

#include "Debug/assert.h"

char
get_short_flag(ArgumentXMacro xm) {
    static const char short_flags[] = {
#define YS(label, _1, short, ...) [label] = short,
        ARGUMENT_XMACRO(YS)
#undef YS
    };

    DEBUG_ASSERT(0 <= xm && xm <= ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_short_flag");

    return short_flags[xm];
}

const char *
get_long_flag(ArgumentXMacro xm) {
    static const char *long_flags[] = {
#define YS(label, _1, _2, long, ...) [label] = long,
        ARGUMENT_XMACRO(YS)
#undef YS
    };

    DEBUG_ASSERT(0 <= xm && xm <= ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_long_flag");

    return long_flags[xm];
}

const char *
get_description(ArgumentXMacro xm) {
    static const char *descriptions[] = {
#define YS(label, desc, ...) [label] = desc,
        ARGUMENT_XMACRO(YS)
#undef YS
    };

    DEBUG_ASSERT(0 <= xm && xm <= ARGUMENT_XMACRO_COUNT,
            "Invalid flag in get_description");

    return descriptions[xm];
}
