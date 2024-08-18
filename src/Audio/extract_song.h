#pragma once

#include <sndfile.h>
#include "Types/Song.h"
#include "Audio/soundfile.h"
#include "App/AppOutput.h"

int
extract_song(SoundFile *src, AppOutput *out,
        double *songbuf, sf_count_t buflen);
