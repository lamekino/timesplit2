#pragma once

#include <sndfile.h>

#include "Types/Song.h"

struct Output {
    const char *output_directory;
    Song *song;
    sf_count_t start;
    sf_count_t end;
};

typedef const struct Output Output;

struct Output
output_create(const char *outdir, Song *cur, Song *next, size_t rate);
