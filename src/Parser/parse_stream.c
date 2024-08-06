#include <wchar.h>

#include "Parser/parse_line.h"
#include "Parser/parse_stream.h"
#include "Song/Song.h"

#define BUFFER_SIZE 1024

int
parse_stream(FILE *stream, struct SongList *dest) {
    int lineno;
    wchar_t linebuf[BUFFER_SIZE] = {0};

    for (lineno = 0; fgetws(linebuf, BUFFER_SIZE, stream); lineno++) {
        struct Song song = parse_line(linebuf, BUFFER_SIZE);

        if (IS_PARSER_SKIP(song)) {
            continue;
        }

        if (IS_PARSER_ERROR(song) || !songlist_push(dest, &song)) {
            return -1;
        }
    }

    return lineno;
}
