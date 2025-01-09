#include <stdio.h>

#include "Args/ArgsXMacro.h"
#include "Args/usage.h"

#define PADDING_LONG 16
#define PADDING_SHORT 4
#define INDENT 4

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

    fprintf(stream, "usage:\n");

    count += fprintf(stream, "%*s", INDENT, " ");
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
            count = fprintf(stream, "\n%*s", INDENT + namelen, " ");
        }
    }

    fprintf(stream, " <path to audio>\n");
}

static void
print_positional_arguments(FILE *stream) {
    int padding = PADDING_SHORT + PADDING_LONG + 3; /* -, -- */

    fprintf(stream, "positional arguments:\n");
    fprintf(stream, "%*s", INDENT, " ");
    fprintf(stream, "%-*s", padding, "<path to audio>");
    fprintf(stream, "the location of the file to splice\n");
}

static void
print_usage_descriptions(FILE *stream) {
    ArgsXMacro xm;

    fprintf(stream, "options:\n");
    for (xm = 0; xm < ARGUMENT_XMACRO_COUNT; xm++) {
        char flagshort = get_short_flag(xm);
        const char *flaglong = get_long_flag(xm);

        fprintf(stream, "    ");
        if (flagshort) {
            int padding =  PADDING_SHORT;
            if (!flaglong) {
                padding = PADDING_SHORT + PADDING_LONG + 2; /* -- */
            }

            fprintf(stream, "-%-*c", padding, flagshort);
        }

        if (flaglong) {
            int padding =  PADDING_LONG;
            if (!flagshort) {
                padding = PADDING_SHORT + PADDING_LONG + 1; /* - */
            }

            fprintf(stream, "--%-*s", padding, flaglong);
        }

        fprintf(stream, "%s\n", get_description(xm));
    }
}

static void
print_synopsis(FILE *stream) {
    const char *timesplit2 = "TimeSplit2";
    fprintf(stream,
            "%s is for slicing sound files, such as breaking a mix"
            "into its individual songs.\n", timesplit2);
}

void
usage(FILE *stream, const char *progname) {
    print_synopsis(stream);
    fprintf(stream, "\n");

    print_usage_header(stream, progname);
    fprintf(stream, "\n");

    print_positional_arguments(stream);
    fprintf(stream, "\n");

    print_usage_descriptions(stream);
}
