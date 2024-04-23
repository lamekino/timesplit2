#pragma once

#ifdef AUDIO_HIDE_IMPL
AUDIO_HIDE_IMPL
#endif

#include "Types/Stack.h"

int audio_interact(const char *audiopath, const struct Stack *parsed);
