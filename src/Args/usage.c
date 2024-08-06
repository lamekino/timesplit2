#include <stdio.h>

#include "Args/arguments_xmacro.h"
#include "Args/usage.h"

#define PADDING_LONG 16
#define PADDING_SHORT 4

static void
print_usage_header(FILE *stream, const char *progname) {
    ArgumentXMacro xm;

    fprintf(stream, "usage:\n");
    fprintf(stream, "%s [", progname);

    for (xm = 1; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        if (xm > 1) fprintf(stream, "|");

        fprintf(stream, "-%c", get_short_flag(xm));
    }

    fprintf(stream, "] filename1 filename2 ...\n");
}

static void
print_usage_descriptions(FILE *stream) {
    ArgumentXMacro xm;

    for (xm = 0; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        fprintf(stream, "    -%-*c %-*s %s\n",
                PADDING_SHORT, get_short_flag(xm),
                PADDING_LONG, get_long_flag(xm),
                get_description(xm));
    }
}

void
usage(FILE *stream, const char *progname) {
    print_usage_header(stream, progname);
    print_usage_descriptions(stream);
}
