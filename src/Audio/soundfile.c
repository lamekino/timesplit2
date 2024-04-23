#include "Audio/soundfile.h"

SNDFILE *
soundfile_open(struct SoundFile *audiofile, const char *path, int mode) {
    audiofile->file = sf_open(path, mode, &audiofile->info);
    return audiofile->file;
}

int
soundfile_close(SoundFile *audiofile) {
    return sf_close(audiofile->file);
}
