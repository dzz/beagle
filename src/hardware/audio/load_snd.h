#ifndef __AUDIO_LOAD_SND__
#define __AUDIO_LOAD_SND__


typedef struct {
    short *data;
    short *left;
    short *right;
    char *name;
    unsigned int len;
    unsigned int smpl_cnt;
    int channels;
    int srate;
    unsigned int playable;
} hw_audio_wav_data;

void audio_load_wav(hw_audio_wav_data* wav, const char* filename);
void audio_drop_wav(hw_audio_wav_data* wav);

#endif
