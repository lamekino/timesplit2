#pragma once

#include "Types/Stack.h"
#include "Types/Error.h"
#include "Args/ArgsConfig.h"

typedef union Error AppMode(const struct ArgsConfig *, const struct Stack *);
