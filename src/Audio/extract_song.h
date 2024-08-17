#pragma once

#include <sndfile.h>
#include "Types/Song.h"
#include "Audio/soundfile.h"
#include "App/Output.h"

int
extract_song(SoundFile *src, Output *out, double *songbuf, sf_count_t buflen);
