#ifndef BGL_BAUDY

void BGLBasicMixer_Init();
void BGLBasicMixer_Shutdown();
void BGLBasicMixer_PlaySound( unsigned int snd);
void BGLBasicMixer_PlayMusic( const char* path);
void BGLBasicMixer_StopMusic();
unsigned int BGLBasicMixer_LoadSound( const char*path);

#endif
