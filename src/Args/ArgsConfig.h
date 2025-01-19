#pragma once

#include <stdlib.h>

struct ArgsConfig {
    char *audio_path;
    char *timestamps_path;
    char *extract_dir;
    size_t thread_count;
};
