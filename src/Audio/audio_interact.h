#pragma once

#ifdef AUDIO_HIDE_IMPL
AUDIO_HIDE_IMPL
#endif

#include "Song/Song.h"

int audio_interact(const char *audiopath, const struct SongList *parsed);
