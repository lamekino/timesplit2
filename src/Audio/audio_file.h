#pragma once

#ifdef AUDIO_HIDE_IMPL
AUDIO_HIDE_IMPL
#endif

#include <sndfile.h>

struct AudioFile {
    SNDFILE *file;
    SF_INFO info;
};

typedef const struct AudioFile AudioFile;

SNDFILE *
audio_open(struct AudioFile *audiofile, const char *path, int mode);

int
audio_close(AudioFile *audiofile);
