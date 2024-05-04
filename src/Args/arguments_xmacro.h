#pragma once

#define ARGUMENT_XMACRO(X) \
X(FLAG_HELP, "view this help", \
    'h', "--help") \
X(FLAG_EXTRACT_ALL, "extract all songs found", \
     'a', "--all")

typedef enum ArgumentXMacro {
#define ENUMERATE(label, ...) label,
    ARGUMENT_XMACRO(ENUMERATE)
#undef ENUMERATE

    ARGUMENT_XMACRO_COUNT
} ArgumentXMacro;

char
get_short_flag(ArgumentXMacro xm);


const char *
get_long_flag(ArgumentXMacro xm);

const char *
get_description(ArgumentXMacro xm);
