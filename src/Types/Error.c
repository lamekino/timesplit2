#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Args/usage.h"
#include "Types/Error.h"

int
error_report(const char *progname, const union Error *err) {
    switch (err->level) {
    case LEVEL_SHOW_HELP: {
        usage(stdout, progname);
        return EXIT_SUCCESS;
    }
    case LEVEL_NO_MEM: {
        fprintf(stderr, "%s: fatal memory error! you need more RAM!!\n",
                progname);
        return EXIT_FAILURE;
    }
    default: {
        if ((char *) ERROR_LEVEL_COUNT < err->description) {
            fprintf(stderr, "%s\n", err->description);
            free(err->description);
            return EXIT_FAILURE;
        }
    }}

    return EXIT_SUCCESS;
}

static union Error
va_error(const char *fmt, va_list ap) {
    char *buf = calloc(1, ERROR_MAX_LEN);

    if (buf == NULL) {
        return error_level(LEVEL_NO_MEM);
    }


    if (vsnprintf(buf, ERROR_MAX_LEN, fmt, ap) < 0) {
        return error_level(LEVEL_NO_MEM);
    }

    return (union Error) {
        .description = buf
    };
}

union Error
error_msg(const char *fmt, ...) {
    union Error err;
    va_list args;

    va_start(args, fmt);
    err = va_error(fmt, args);
    va_end(args);

    return err;
}

