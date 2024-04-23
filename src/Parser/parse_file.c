#include <stdio.h>

#include "Parser/parse_stream.h"

int
parse_file(const char *filepath, struct SongList *dest) {
    int lines_parsed;
    FILE *fp = fopen(filepath, "r");

    if (!fp) {
        return -1;
    }

    lines_parsed = parse_stream(fp, dest);
    fclose(fp);

    return lines_parsed;
}
