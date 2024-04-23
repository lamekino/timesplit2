#include <wchar.h>

#include "Parser/parse_line.h"
#include "Parser/parse_stream.h"
#include "Types/Stack.h"

#define BUFFER_SIZE 1024

int
parse_stream(FILE *stream, struct Stack *dest) {
    int lineno = 1;
    wchar_t linebuf[BUFFER_SIZE] = {0};

    while ((fgetws(linebuf, BUFFER_SIZE, stream))) {
        struct Song song;

        if (wcsncmp(linebuf, L"\n", BUFFER_SIZE) == 0) continue;

        song = parse_line(linebuf, BUFFER_SIZE);

        if (IS_PARSE_ERROR(song)) {
            return -1;
        }

        if (!stack_push(dest, &song, sizeof(struct Song))) {
            return -1;
        }

        lineno++;
    }

    return lineno;
}
