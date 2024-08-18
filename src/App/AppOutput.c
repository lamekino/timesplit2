#include "App/AppOutput.h"
#include "Audio/song_frame_offset.h"

struct AppOutput
app_output_create(const char *outdir, Song *cur, Song *next, size_t rate) {
    return (AppOutput) {
        .song = cur,
        .output_directory = outdir,
        .start = song_frame_offset(cur, rate),
        .end = song_frame_offset(next, rate),
    };
}
