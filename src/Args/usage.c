#include <stdio.h>

#include "Args/ArgsXMacro.h"
#include "Args/usage.h"

#define PADDING_LONG 16
#define PADDING_SHORT 4

static int
print_flag_arg_info(FILE *stream, const struct FlagInfo *info) {
    if (info->argcount == 1) {
        return fprintf(stream, " <%s>", info->argtype);
    }

    if (info->argcount == FLAG_INFO_INDEFINITE) {
        return fprintf(stream, " <%s 1> ... <%s N>",
                info->argtype, info->argtype);
    }

    return 0;
}

static void
print_usage_header(FILE *stream, const char *progname) {
    const size_t threshold = 80;
    int count = 0;
    int namelen = 0;

    ArgsXMacro xm;

    count += namelen = fprintf(stream, "%s", progname);

    for (xm = 1; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        char flagshort = get_short_flag(xm);
        const char *flaglong = get_long_flag(xm);
        struct FlagInfo flaginfo = get_flag_info(xm);

        count += fprintf(stream, " [");

        if (flagshort) {
            count += fprintf(stream, "-%c", flagshort);
            count += print_flag_arg_info(stream, &flaginfo);
        }

        if (flagshort && flaglong) {
            count += fprintf(stream, " | ");
        }

        if (flaglong) {
            count += fprintf(stream, "--%s", flaglong);
            count += print_flag_arg_info(stream, &flaginfo);
        }

        count += fprintf(stream, "]");
        if ((size_t) count >= threshold) {
            count = fprintf(stream, "\n%-*s", namelen, "");
        }
    }

    fprintf(stream, " <path to audio>\n");
}

static void
print_usage_descriptions(FILE *stream) {
    ArgsXMacro xm;

    fprintf(stream, "positional arguments:\n");
    fprintf(stream, "    %-*s %s\n",
            PADDING_LONG, "<path to audio>",
            "the location of the file to splice");
    fprintf(stream, "\n");

    fprintf(stream, "options:\n");
    for (xm = 0; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        char flagshort = get_short_flag(xm);
        const char *flaglong = get_long_flag(xm);

        fprintf(stream, "    ");
        if (flagshort) {
            int padding = flaglong
                ? PADDING_SHORT
                : PADDING_SHORT + PADDING_LONG + 2;

            fprintf(stream, "-%-*c", padding, flagshort);
        }

        if (flaglong) {
            int padding = flagshort
                ? PADDING_LONG
                : PADDING_LONG + PADDING_SHORT + 1;

            fprintf(stream, "--%-*s", padding, flaglong);
        }

        fprintf(stream, "%s\n", get_description(xm));
    }
}

void
usage(FILE *stream, const char *progname) {
    print_usage_header(stream, progname);

    fprintf(stream, "\n%s can be used to slice audio formats into smaller "
            "files, ie breaking a mix into its individual songs.\n\n",
            progname);

    print_usage_descriptions(stream);
}
