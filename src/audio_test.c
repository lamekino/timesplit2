#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_mixer.h>

#define AUDIO_TEST
#ifdef AUDIO_TEST
#define audio_test main
#else
#define audio_test audio_test
#endif

int audio_test() {
    return 0;
}
