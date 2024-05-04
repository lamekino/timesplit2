#include <assert.h> /* TODO: debug assert macro */
#include "Args/arguments_xmacro.h"

char
get_short_flag(ArgumentXMacro xm) {
    switch (xm) {
#define CASE(label, _1, short, ...) case label: return short;
        ARGUMENT_XMACRO(CASE)
#undef CASE
        default: break;
    }

    /* TODO: throw something like an exception to main */
    assert(0 && "unreacable");
    return 0;
}

const char *
get_long_flag(ArgumentXMacro xm) {
    switch (xm) {
#define CASE(label, _1, _2, long, ...) case label: return long;
        ARGUMENT_XMACRO(CASE)
#undef CASE
        default: break;
    }

    /* TODO: throw something like an exception to main */
    assert(0 && "unreacable");
    return 0;
}

const char *
get_description(ArgumentXMacro xm) {
    switch (xm) {
#define CASE(label, desc, ...) case label: return desc;
        ARGUMENT_XMACRO(CASE)
#undef CASE
        default: break;
    }

    /* TODO: throw something like an exception to main */
    assert(0 && "unreacable");
    return 0;
}
