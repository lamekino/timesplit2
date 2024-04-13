#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "parser/parser.h"
#include "types/stack.h"
#include "types/song.h"
#include "types/resource.h"

int main() {
    const char *filename = "./sample-input/MzCEqlPp0L8";
    const char *localename = "en_US.UTF-8";

    struct ResoureArena arena = {0};

    FILE *fp = NULL;
    struct Stack data = {0};

    /* TODO: ensure that defers get freed on sigterm */
    if (!defer_file_resource(&arena, &fp, filename, "r")) {
        report_fatal_error(&arena, "could not open file %s", filename);
    }

    if (!defer_stack_resource(&arena, &data)) {
        report_fatal_error(&arena, "could not create data stack");
    }

    if (!setlocale(LC_ALL, localename)) {
        report_fatal_error(&arena, "could not set locale to %s", localename);
    }

    if (parse_stream(fp, &data) < 0) {
        report_fatal_error(&arena, "error in parsing");
    }

    {
        size_t i;
        for (i = 0; i < data.count; i++) {
            struct Song *cur = data.elems[i];
            printf("L\"%ls\": %ld\n", cur->title, cur->timestamp);
        }
    }

    free_all_resources(&arena);
    return EXIT_SUCCESS;
}
