#pragma once

/*
 * NOTE: we maybe able to include the option to make the short flag or the long
 * flag optional (but shouldn't be both)
 *
 * we add a:
 * X(FLAG_NULL, NULL, '\0', NULL)
 * to the top of the xmacro definition, so when code gets generated any lookup
 * should resolve to the FLAG_NULL label
 */

/*
 * TODO: swap short and long flag and format like
 * X( \
 *     label, \
 *     desc, \
 *     long, \
 *     short, \
 * )
 */
#define ARGUMENT_XMACRO(X) \
X(FLAG_HELP, "view this help", \
    'h', "--help") \
X(FLAG_EXTRACT_ALL, "extract all songs found", \
     'a', "--all") \
X(FLAG_EXTRACT_OUTPUT, "[TODO] set the directory for songs to be extracted to", \
     'o', "--output") \
X(FLAG_TIMESTAMPS_FILE, "[TODO] use a file to read timestamps instead of stdin", \
     'f', "--timestamps")

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
