#pragma once

#include <stdbool.h>

struct ArgConfig {
    bool use_extract_all;

    char *audio_path;
    char *timestamps_path;
    char *extract_dir;
};
