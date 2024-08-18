#pragma once

#include <sndfile.h>
#include "Types/Song.h"
#include "Audio/AudioFile.h"
#include "App/AppOutput.h"

int
extract_song(AudioFile *src, AppOutput *out,
        double *songbuf, sf_count_t buflen);
