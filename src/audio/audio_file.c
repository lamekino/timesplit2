#include "audio/audio_file.h"

SNDFILE *
audio_open(struct AudioFile *audiofile, const char *path, int mode) {
    audiofile->file = sf_open(path, mode, &audiofile->info);
    return audiofile->file;
}

int
audio_close(AudioFile *audiofile) {
    return sf_close(audiofile->file);
}
