#pragma once

#ifdef PARSER_HIDE_IMPL
PARSER_HIDE_IMPL
#endif

#include "Types/Stack.h"

int
parse_file(const char *filepath, struct Stack *dest);
