#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "audio/audio_interact.h"
#include "parser/parse_file.h"
#include "types/stack.h"
#include "types/song.h"
#include "types/resource.h"


int main(int argc, char *argv[]) {
    const char *filename = "./sample-input/MzCEqlPp0L8";
    const char *audiopath = "./sample-audio/MzCEqlPp0L8.mp3";
    const char *localename = "en_US.UTF-8";

    struct Stack parsed = {0};

    (void) argc; (void) argv;

    if (!stack_create(&parsed)) {
        fprintf(stderr, "could not create data stack\n");
        goto FAIL;
    }

    if (!setlocale(LC_ALL, localename)) {
        fprintf(stderr, "could not set locale to %s\n", localename);
        goto FAIL;
    }

    /* TODO: process arguments */

    if (parse_file(filename, &parsed) < 0) {
        fprintf(stderr, "error in parsing\n");
        goto FAIL;
    }

    /* TODO: better errors for this part of the program */
    if (audio_interact(audiopath, &parsed) < 0) {
        fprintf(stderr, "error in audio?? bad error\n");
        goto FAIL;
    }

    stack_cleanup(&parsed, free_song);
    return EXIT_SUCCESS;
FAIL:
    stack_cleanup(&parsed, free_song);
    return EXIT_FAILURE;

}
