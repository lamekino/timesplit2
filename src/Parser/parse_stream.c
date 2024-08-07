#include <wchar.h>

#include "Parser/parse_line.h"
#include "Parser/parse_stream.h"
#include "Song/song_type.h"
#include "Song/stack.h"

#define BUFFER_SIZE 1024

int
parse_stream(FILE *stream, struct Stack *dest) {
    int lineno;
    wchar_t linebuf[BUFFER_SIZE] = {0};

    for (lineno = 0; fgetws(linebuf, BUFFER_SIZE, stream); lineno++) {
        struct Song song = parse_line(linebuf, BUFFER_SIZE);

        if (IS_PARSER_SKIP(song)) {
            continue;
        }

        if (IS_PARSER_ERROR(song) || !stack_push(dest, &song)) {
            return -1;
        }
    }

    return lineno;
}
