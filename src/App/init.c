#include <locale.h>

#include "Types/Error.h"

#define STACK_IMPL
#include "Types/Stack.h"

union Error
init(const char *localename, int argc, struct Stack *mem) {
    if (argc < 2) {
        return error_level(LEVEL_FAILED);
    }

    if (!stack_create(mem)) {
        return error_level(LEVEL_NO_MEM);
    }

    if (!setlocale(LC_ALL, localename)) {
        return error_msg("could not set locale to '%s'", localename);
    }

    return error_level(LEVEL_SUCCESS);
}

