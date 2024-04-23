#pragma once

#ifdef AUDIO_HIDE_IMPL
AUDIO_HIDE_IMPL
#endif

#include <sndfile.h>
#include "Types/Song.h"
#include "Audio/audio_file.h"

int
audio_extract(AudioFile *src, Song *song, sf_count_t position,
        sf_count_t ending, double *songbuf, sf_count_t buflen);
