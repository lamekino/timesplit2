#include "App/AppOutput.h"
#include "Audio/song_frame_offset.h"

struct AppOutput
app_output_create(const char *outdir, Song *cur, Song *next, AudioFile *src) {
    struct AppOutput y = (AppOutput) {
        .song = cur,
        .output_directory = outdir,
        .start = song_frame_offset(cur, src->info.samplerate),
        .end = song_frame_offset(next, src->info.samplerate)
    };

    if (next->timestamp < cur->timestamp) {
        y.end = src->info.frames;
    }

    return y;
}
