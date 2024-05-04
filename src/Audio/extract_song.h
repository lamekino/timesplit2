#pragma once

#include <sndfile.h>
#include "Song/Song.h"
#include "Audio/soundfile.h"

int
extract_song(SoundFile *src, Song *song, sf_count_t position,
        sf_count_t ending, double *songbuf, sf_count_t buflen);
