#include <sndfile.h>

#include "Types/Song.h"

sf_count_t
song_frame_offset(Song *song, int samplerate);
