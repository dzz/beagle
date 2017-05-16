#include "load_snd.h"
#include "../../system/rt_module_codes.h"
#include "../../system/log.h"

#include "ogg.h"
#include <malloc.h>

static void _default_wav(hw_audio_wav_data* wav) {
    wav->data = 0;
    wav->name = "";
    wav->len = 0;
    wav->channels = 0;
    wav->srate = 0;
    wav->smpl_cnt = 0;
    wav->playable = 0;
    wav->left=0;
    wav->right=0;
}


void audio_load_wav(hw_audio_wav_data* wav, const char* filename)  {
    unsigned int i;
    _default_wav(wav);
    wav->len = stb_vorbis_decode_filename( filename, &wav->channels, &wav->srate, &wav->data );
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "[ogg]name:%s", filename );
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "[ogg]chan:%x", wav->channels);
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "[ogg]rate:%x", wav->srate);
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "[ogg]pntr:%x", wav->data);
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "[ogg]leng:%x", wav->len);
    if(wav->channels!=2) {
        log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_ERROR, "[ogg][err] only support stereo oggs", filename );
        return;
    }

    wav->smpl_cnt = wav->len / wav->channels;
    wav->left = (short*)malloc((sizeof(short)*wav->smpl_cnt));
    wav->right = (short*)malloc((sizeof(short)*wav->smpl_cnt));
   
    {
        unsigned int ptr = 0;
        for( i=0; i< wav->smpl_cnt; ++i) {

            wav->left[i] = wav->data[ptr];
            ptr++;
            wav->right[i] = wav->data[ptr];
            ptr++;
            
        }
    }
}

void audio_drop_wav(hw_audio_wav_data* wav) {
    if(wav->data!=0) free(wav->data); wav->data=0;
    if(wav->left!=0) free(wav->left); wav->left=0;
    if(wav->right!=0) free(wav->right); wav->right=0;
    wav->name="";
    wav->len=0;
    wav->channels=0;
    wav->srate =0;
}


