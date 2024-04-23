#ifndef AUDIO_FILE_H
#define AUDIO_FILE_H

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

#endif /* AUDIO_FILE_H */
