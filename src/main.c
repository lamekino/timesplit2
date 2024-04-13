#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "parser/parser.h"
#include "types/stack.h"
#include "types/song.h"

#define BUFFER_SIZE 1024

#define EXIT_WHEN(p, blame) \
    do { \
        if (p) { \
            perror(blame); \
            exit(EXIT_FAILURE); \
        } \
    } while (0);

int main() {
    size_t i = 0;

    wchar_t line_buf[BUFFER_SIZE] = {0};
    struct Stack data = {0};
    FILE *fp = fopen("./sample-input/MzCEqlPp0L8", "r");

    /* TODO: defer */
    EXIT_WHEN(!fp, "fopen");
    EXIT_WHEN(!setlocale(LC_ALL, "en_US.UTF-8"), "setlocale");
    EXIT_WHEN(!stack_create(&data), "malloc");

    while ((fgetws(line_buf, BUFFER_SIZE, fp))) {
        struct Song song;
        if (wcsncmp(line_buf, L"\n", BUFFER_SIZE) == 0) continue;

        song = parse_line(line_buf, BUFFER_SIZE);

        if (IS_PARSE_ERROR(song)) {
            break;
        }

        if (!stack_push(&data, &song, sizeof(struct Song))) {
            break;
        }
    }

    for (i = 0; i < data.count; i++) {
        struct Song *cur = data.elems[i];
        printf("L\"%ls\": %ld\n", cur->title, cur->timestamp);
    }

    stack_cleanup(&data, free_song);
    fclose(fp);

    return EXIT_SUCCESS;
}
