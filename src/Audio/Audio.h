#pragma once

#include <sndfile.h>

#include "Audio/audio_interact.h"
#include "Audio/audio_file.h"

#define AUDIO_HIDE_IMPL \
    typedef char module_error[-1];
