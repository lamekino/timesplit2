#include <wchar.h>

#include "Parser/parse_line.h"
#include "Parser/parse_stream.h"
#include "Types/Error.h"
#include "Types/Song.h"

#define STACK_IMPL
#include "Types/Stack.h"

#define BUFFER_SIZE 1024

union Error
parse_stream(FILE *stream, struct Stack *dest) {
    int lineno;
    wchar_t linebuf[BUFFER_SIZE] = {0};

    for (lineno = 0; fgetws(linebuf, BUFFER_SIZE, stream); lineno++) {
        struct Song song = parse_line(linebuf, BUFFER_SIZE);

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
