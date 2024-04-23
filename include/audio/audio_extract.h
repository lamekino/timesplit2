#ifndef AUDIO_EXTRACT_H
#define AUDIO_EXTRACT_H

#include <sndfile.h>
#include "types/song.h"
#include "audio/audio_file.h"

int
audio_extract(AudioFile *src, Song *song, sf_count_t position,
        sf_count_t ending, double *songbuf, sf_count_t buflen);

#endif /* AUDIO_EXTRACT_H */
