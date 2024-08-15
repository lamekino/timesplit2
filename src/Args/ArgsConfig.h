#pragma once

#include "App/AppMode.h"

struct ArgsConfig {
    char *audio_path;
    char *timestamps_path;
    char *extract_dir;

    AppMode *interact;
};
