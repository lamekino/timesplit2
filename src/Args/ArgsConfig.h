#pragma once

#include "Audio/AudioInteract.h"

struct ArgsConfig {
    char *audio_path;
    char *timestamps_path;
    char *extract_dir;

    AudioInteract *interact;
};
