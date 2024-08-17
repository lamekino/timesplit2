#include <wchar.h>

#include "Parser/parse_line.h"
#include "Parser/parse_stream.h"
#include "Types/Error.h"
#include "Types/Song.h"
#include "Types/Stack.h"

#define BUFFER_SIZE 1024

union Error
parse_stream(FILE *stream, struct Stack *dest) {
    const size_t max_write = BUFFER_SIZE - 1;

    int lineno;
    wchar_t linebuf[BUFFER_SIZE] = {0};

    for (lineno = 0; fgetws(linebuf, max_write, stream); lineno++) {
        struct Song song = parse_line(linebuf);

        if (IS_PARSER_SKIP(song)) {
            continue;
        }

        /* TODO: make parser error handle more cases such as running out of
         * memory */
        if (IS_PARSER_ERROR(song)) {
            return error_msg("failed to parse line %d, '%s'", lineno, linebuf);
        }

        if (!stack_push(dest, &song)) {
            return error_level(LEVEL_NO_MEM);
        }
    }

    return error_level(LEVEL_SUCCESS);
}
