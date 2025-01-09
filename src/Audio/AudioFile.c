#include "Audio/AudioFile.h"

SNDFILE *
audiofile_open(struct AudioFile *audiofile, const char *path, int mode) {
    /* open the file and record the info */
    audiofile->file = sf_open(path, mode, &audiofile->info);

    /* find the length and then rewind the file */
    audiofile->len = sf_seek(audiofile->file, 0, SEEK_END);
    sf_seek(audiofile->file, 0, SEEK_SET);

    return audiofile->file;
}

int
audiofile_close(AudioFile *audiofile) {
    return sf_close(audiofile->file);
}
