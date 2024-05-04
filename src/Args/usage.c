#include <stdio.h>

#define XMACRO_INCLUDE_ALL_GLOBALS
#define XMACRO_TERMINATE_LISTS

#include "Args/arguments_xmacro.h"
#include "Args/usage.h"

#define PADDING_DESC 20
#define PADDING_LONG 20
#define PADDING_SHORT 10

static void
print_usage_header(FILE *stream, const char *progname) {
    ArgumentXMacro xm;

    fprintf(stream, "usage:\n");
    fprintf(stream, "%s [", progname);

    for (xm = 1; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        if (xm > 1) printf("|");

        printf("-%c", get_short_flag(xm));
    }

    fprintf(stream, "] filename1 filename2 ...\n");
}

static void
print_usage_descriptions(FILE *stream) {
    ArgumentXMacro xm;

    for (xm = 0; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        fprintf(stream, "    -%-*c %-*s %-*s\n",
                PADDING_SHORT, get_short_flag(xm),
                PADDING_LONG, get_long_flag(xm),
                PADDING_DESC, get_description(xm));
    }
}

void
usage(FILE *stream, const char *progname) {
    print_usage_header(stream, progname);
    print_usage_descriptions(stream);
}
