#include <sndfile.h>

#include "App/AppOutput.h"
#include "Audio/soundfile.h"
#include "Audio/extract_song.h"
#include "Audio/song_frame_offset.h"
#include "Types/Error.h"
#include "App/app_extract_all.h"
#include "Types/Stack.h"

#define LENGTH(xs) (sizeof(xs)/sizeof(*xs))

union Error
app_extract_all(const char *outdir, const char *audiopath,
        const struct Stack *ts) {
    struct SoundFile snd = {0};
    size_t idx;

    double songbuf[4096];
    const size_t buflen = LENGTH(songbuf);

    if (!soundfile_open(&snd, audiopath, SFM_READ)) {
        return error_msg("could not open '%s': %s",
                audiopath, sf_strerror(snd.file));
    }

    for (idx = 0; idx < ts->count; idx++) {
        Song *cur = stack_mod_index(ts, idx);
        Song *next = stack_mod_index(ts, idx + 1);

        AppOutput out =
            app_output_create(outdir, cur, next, snd.info.samplerate);

        printf("Extracting: '%ls'\n", cur->title);

        if (extract_song(&snd, &out, songbuf, buflen) < 0) {
            return error_msg("failed to extract '%s'", cur->title);
        }
    }

    return error_level(LEVEL_SUCCESS);
}
