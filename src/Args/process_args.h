#pragma once

#include "App/AppMode.h"
#include "Args/ArgsConfig.h"
#include "Types/Error.h"

union Error
process_args(char *argv[], AppMode **interact, struct ArgsConfig *config);
