#pragma once

#include <sndfile.h>

#include "Types/Song.h"

struct AppOutput {
    const char *output_directory;
    Song *song;
    sf_count_t start;
    sf_count_t end;
};

typedef const struct AppOutput AppOutput;

struct AppOutput
app_output_create(const char *outdir, Song *cur, Song *next, size_t rate);
