#pragma once

#ifdef AUDIO_HIDE_IMPL
AUDIO_HIDE_IMPL
#endif

#include <sndfile.h>

struct SoundFile {
    SNDFILE *file;
    SF_INFO info;
};

typedef const struct SoundFile SoundFile;

SNDFILE *
soundfile_open(struct SoundFile *audiofile, const char *path, int mode);

int
soundfile_close(SoundFile *audiofile);
