#include <string.h>

#include "Args/arguments_xmacro.h"
#include "Args/usage.h"

struct ShortFlag {
    char flag[3];
};

static struct ShortFlag
short_flag_string(ArgumentXMacro xm) {
    return (struct ShortFlag) {
        { '-', get_short_flag(xm), '\0' }
    };
}

int
process_args(int argc, char *argv[]) {
    int i;

    for (i = 1; i < argc; i++) {
        const size_t arg_len = strlen(argv[i]);

        if (arg_len <= 1) {
            return -1;
        }

        if (strncmp(argv[i], "-h", arg_len) == 0) {
            usage(stdout, argv[0]);
            return -1;
        }
    }

    return 0;
}
