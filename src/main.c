#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "parser/parser.h"
#include "types/stack.h"
#include "types/song.h"
#include "types/resource.h"

#define BUFFER_SIZE 1024

int main() {
    const char *filename = "./sample-input/MzCEqlPp0L8";
    size_t lineno = 1;
    size_t i;

    struct ResoureArena arena = {0};

    wchar_t linebuf[BUFFER_SIZE] = {0};
    struct Stack data = {0};

    FILE *fp = defer_file_resource(&arena, filename, "r");
    if (!fp) {
        report_fatal_error(&arena, "could not open file %s", filename);
    }

    if (!defer_stack_resource(&arena, &data)) {
        report_fatal_error(&arena, "could not create data stack");
    }

    if (!setlocale(LC_ALL, "en_US.UTF-8")) {
        report_fatal_error(&arena, "could not set locale");
    }

    while ((fgetws(linebuf, BUFFER_SIZE, fp))) {
        struct Song song;

        if (wcsncmp(linebuf, L"\n", BUFFER_SIZE) == 0) continue;

        song = parse_line(linebuf, BUFFER_SIZE);

        if (IS_PARSE_ERROR(song)) {
            report_fatal_error(&arena, "could not parse line %ld: \"%ls\"",
                    lineno, linebuf);
        }

        if (!stack_push(&data, &song, sizeof(struct Song))) {
            report_fatal_error(&arena, "could not push to stack");
        }

        lineno++;
    }

    for (i = 0; i < data.count; i++) {
        struct Song *cur = data.elems[i];
        printf("L\"%ls\": %ld\n", cur->title, cur->timestamp);
    }

    free_all_resources(&arena);
    return EXIT_SUCCESS;
}
