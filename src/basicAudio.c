#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>


#define BAUDY_MAX_SNDS (256)
Mix_Chunk *baudy_sounds[BAUDY_MAX_SNDS];

unsigned int baudy_snd_count = 0;

void BGLBasicMixer_Init() {
    printf("[Baudy Is Present]\n");
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT,2, 4096);
}

unsigned int BGLBasicMixer_LoadSound( const char*path) {
	printf("baudy loading %s", path);
    baudy_sounds[baudy_snd_count] = Mix_LoadWAV(path);
    return baudy_snd_count++;
}

void BGLBasicMixer_PlaySound( unsigned int snd) {
    printf("playing sound %i\n",snd);
    if(snd<baudy_snd_count) {
		printf("addr %x\n", baudy_sounds[snd]);
        int r = Mix_PlayChannel(-1, baudy_sounds[snd],0);
		printf("result: %i\n", r);
    }
}

void BGLBasicMixer_Shutdown() {

    for(unsigned int i = 0; i<baudy_snd_count; ++i) {
        Mix_FreeChunk(baudy_sounds[i]);
    }

    Mix_CloseAudio();
    printf("[Baudy Has Gone Away]\n");
}
