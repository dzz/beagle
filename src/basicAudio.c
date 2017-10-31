#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>


#define BAUDY_MAX_SNDS (256)
#define BAUDY_MAX_MUSIC (64)

static Mix_Chunk *baudy_sounds[BAUDY_MAX_SNDS];
static Mix_Music *music;

unsigned int baudy_snd_count = 0;

void BGLBasicMixer_Init() {
    printf("[Baudy Is Present]\n");
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init( MIX_INIT_OGG );
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT,2, 512);
    Mix_AllocateChannels(256);
}


void BGLBasicMixer_PlayMusic( const char*path) {

    if(music>0)
        Mix_FreeMusic(music);

    music = Mix_LoadMUS(path);
    Mix_PlayMusic(music,-1);
}

void BGLBasicMixer_StopMusic() {

    Mix_HaltMusic();
    if(music>0)
        Mix_FreeMusic(music);
}

unsigned int BGLBasicMixer_LoadSound( const char*path) {
    baudy_sounds[baudy_snd_count] = Mix_LoadWAV(path);
    return baudy_snd_count++;
}

void BGLBasicMixer_PlaySound( unsigned int snd) {
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

    BGLBasicMixer_StopMusic();
    Mix_CloseAudio();
    printf("[BASIC AUDIO Has Gone Away]\n");
}
