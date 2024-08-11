#include <stdio.h>

#include "Parser/parse_file.h"
#include "Parser/parse_stream.h"
#include "Types/Error.h"

union Error
parse_file(const char *filepath, struct Stack *dest) {
    union Error lvl;
    FILE *fp = NULL;

    if (filepath == NULL) {
#if 0
        return error_msg("no timestamps provided");
#else
        fprintf(stderr, "No timestamp file provided, paste them here:\n");
        return parse_stream(stdin, dest);
#endif
    }

    fp = fopen(filepath, "r");
    if (!fp) {
        return error_msg("Couldn't open file: '%s'", filepath);
    }

    lvl = parse_stream(fp, dest);
    fclose(fp);

    return lvl;
}
