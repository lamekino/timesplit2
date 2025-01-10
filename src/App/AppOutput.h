#pragma once

#include <sndfile.h>

#include "Audio/AudioFile.h"
#include "Types/Song.h"

struct AppOutput {
    const char *output_directory;
    Song *song;
    sf_count_t start;
    sf_count_t end;
};

typedef const struct AppOutput AppOutput;

struct AppOutput
app_output_create(const char *outdir, Song *cur, Song *next, AudioFile *src);
