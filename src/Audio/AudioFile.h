#pragma once

#include <sndfile.h>

struct AudioFile {
    SNDFILE *file;
    SF_INFO info;
    sf_count_t len;
};

typedef const struct AudioFile AudioFile;

SNDFILE *
audiofile_open(struct AudioFile *audiofile, const char *path, int mode);

int
audiofile_close(AudioFile *audiofile);
