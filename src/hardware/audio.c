
#include "../memory.h"
#include "../system/log.h"
#include "audio/load_snd.h"
#include <stdio.h>

#if defined(__STDC__)
# define C89
# if defined(__STDC_VERSION__)
#  define C90
#  if (__STDC_VERSION__ >= 199409L)
#   define C94
#  endif
#  if (__STDC_VERSION__ >= 199901L)
#   define C99
#  endif
# endif
#endif


#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

#include "audio.h"
#include "hf_timer.h"

#include <math.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_thread.h>

#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
#include <portaudio.h>
#endif

#include "../system/rt_module_codes.h"

#define MAX_BACKDOORS 256

float backdoors[MAX_BACKDOORS];

void ae_set_backdoor(unsigned int idx,float value) {
    backdoors[idx]  = value;
}

SDL_Thread *ae_thread;

typedef struct {
    float time;
} ae_renderer_context;

#ifndef BEAGLE_USE_PORTAUDIO_ENGINE
typedef void* PaStream;
typedef void* PaStreamCallbackTimeInfo;
typedef void* PaStreamCallbackFlags;
#endif

typedef struct {
    int shutdown;
    int initialized;
    int finalized;
    PaStream* stream;
    ae_renderer_context context;
} ae_data;

ae_data AE_Data;

static float delta = 1.0f / 44100.0f;
static float time = 0.0f;
unsigned long smpl;


seq_sequencer* Sequencer;

void sequencer_track_create( seq_track* track) {
    track->cur_sample = 0;
    track->active_event = 0;
    track->next_event = 0;
    track->control_message = 0;
    track->old_event = 0;
    track->beat_lock = beat_half;
}
void sequencer_track_set_bpm( seq_track* track, unsigned int bpm) {

    double d_bps = ((double)bpm)/60.0;
    double d_spb = (double)AE_SAMPLERATE / d_bps;
    double d_spbl = 0.0;
    switch(track->beat_lock) {
        case beat_eigth:
            d_spbl = d_spb*0.5;
            break;
        case beat_quarter:
            d_spbl = d_spb;
        case beat_half:
            d_spbl = d_spb*2.0;
        case beat_whole:
            d_spbl = d_spb*4.0;
        default:
            break;
    }
    track->beat_lock_mod = (unsigned int)d_spbl;
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "Set Track %x beat lock mod to %x", track, track->beat_lock_mod);
}

void sequencer_set_bpm(seq_sequencer* seq, unsigned int bpm) {
    seq->bpm = bpm;
    {
        unsigned int i; 
        for(i=0; i< seq->n_tracks;++i) {
            sequencer_track_set_bpm( &seq->tracks[i], bpm );
        }
    }
}


void sequencer_create(seq_sequencer* seq) {
    seq->tracks = (seq_track*)malloc(sizeof(seq_track) * SEQUENCER_TRACKS);
    seq->n_tracks = SEQUENCER_TRACKS;
    seq->halt = 1;
    {
        unsigned int i; 
        for(i=0; i< SEQUENCER_TRACKS;++i) {
            sequencer_track_create( &seq->tracks[i] );
        }
    }
    sequencer_set_bpm( seq, 128 );
    seq->halt = 0;
}

static void sequencer_destroy_active_message(unsigned int tr_id) {
    seq_track* track = &(Sequencer->tracks[tr_id]);
    if( track->control_message!=0) {
        seq_track_msg* control_message = track->control_message; 
        track->control_message = 0;
        if(control_message->data!=0) {
            free(control_message->data);
        }
        free(control_message);
    }
}
static void sequencer_destroy_old_event(seq_track* track) {
    if(track->old_event != 0 ) {
        void* hndl = (void*)track->old_event;
        track->old_event = 0;
        free(hndl);
    }
}

static void sequencer_destroy_active_event(seq_track* track) {
    if(track->active_event != 0 ) {
        void* hndl = (void*)track->active_event;
        track->active_event = 0;
        free(hndl);
    }
}
static void sequencer_destroy_next_event(seq_track* track) {
    if(track->next_event != 0 ) {
        void* hndl = (void*)track->next_event;
        track->next_event = 0;
        free(hndl);
    }
}

void sequencer_drop(seq_sequencer* seq) {
    unsigned int i;
    for(i=0; i< SEQUENCER_TRACKS; ++i) {
       sequencer_destroy_active_message(i);
       sequencer_destroy_active_event(&seq->tracks[i]);
       sequencer_destroy_next_event(&seq->tracks[i]);
       sequencer_destroy_old_event(&seq->tracks[i]);
    }
    free(seq->tracks);
}


void sequencer_halt() {
#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
    Sequencer->halt = 1;
#endif
}

void sequencer_issue_message( unsigned int track, seq_track_msg* message) {
    sequencer_destroy_active_message(track);
    Sequencer->tracks[track].control_message = message;
}

void sequencer_set_next_track_event( seq_track* track, seq_track_event* event) {
    sequencer_destroy_next_event(track);
    track->next_event = event;
    log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "Sequencer track %x next event set to %x", track->next_event );
}

void sequencer_handle_messages() {
    unsigned int trk_id;
    //l    og_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_DEBUG, "Sequencer control thread checking messages");
    for(trk_id=0; trk_id<Sequencer->n_tracks;++trk_id) {
        seq_track* track = &Sequencer->tracks[trk_id];
        sequencer_destroy_old_event(track);
        if(track->control_message!=0) {
                seq_track_msg* msg = track->control_message;
                //log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_DEBUG, "Sequencer Received Message %x", track->control_message );
                switch(msg->code) {
                    case msg_set_event:
                        sequencer_set_next_track_event( track, (seq_track_event*)msg->data );
                        msg->data = 0; 
                        break;
                    default:
                        break;
                }
                sequencer_destroy_active_message(trk_id);
        }
    }
}


void sequencer_queue_wav( unsigned int track, hw_audio_wav_data* wav) {
    seq_track_event* event = (seq_track_event*)malloc(sizeof(seq_track_event));
    seq_track_msg* control_message = (seq_track_msg*)malloc(sizeof(seq_track_msg));
    event->evt_type = evt_loop;
    event->wav = wav;
    event->loop_smpls = 128;
    control_message->code = msg_set_event;
    control_message->data = event;
    sequencer_issue_message( track, control_message );
}


static void InitSequencer() {
    log_message(CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "Sequencer bootup...");
    Sequencer = (seq_sequencer*)malloc(sizeof(seq_sequencer));
    sequencer_create( Sequencer );
}

static void DropSequencer() {
    log_message(CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "Sequencer shutdown...");
    sequencer_drop(Sequencer);
    free(Sequencer);
}

static int ae_renderer( const void* inputBuffer, void* outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void* userData ) {


  
    float* out = (float*)outputBuffer;
    unsigned int tr_idx;
    unsigned int i;
    for(i=0; i<framesPerBuffer; ++i ) {

        float left = 0.0f;
        float right = 0.0f;
        if(Sequencer->halt==0)
        for(tr_idx=0; tr_idx<Sequencer->n_tracks;++tr_idx) {
                seq_track* track = &Sequencer->tracks[tr_idx];
                
                //if there's a pending event, and the GC has room, 
                if( (track->next_event!=0) && (track->old_event==0) ) {
                    //and if we're on a beatlocked sample
                    if( (track->cur_sample % track->beat_lock_mod) ==0) {
                        //swap the tapes
                        log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_AUDMSG, "[rdr] swapping tapes");
                        track->old_event = track->active_event;
                        track->active_event = track->next_event;
                        track->next_event = 0;
                        track->cur_sample = 0;
                    }
                }
                //log_message( CTT2_RT_MODULE_AUDIO, LOG_LEVEL_AUDMSG, "trackptr%x, track:%x sample:%x", track, tr_idx, track->cur_sample );
                if(track->active_event != 0 ) {
                    seq_track_event* evt = track->active_event;
                    unsigned int idx_head = track->cur_sample % evt->wav->smpl_cnt;
                    left  += (float)(evt->wav->left[idx_head]) / 32767.0f;
                    right += (float)(evt->wav->right[idx_head]) / 32767.0f;
                }
                track->cur_sample++;
            }
        *out++ = left;
        *out++ = right;
    }

    /*
            unsigned int i;
            ae_renderer_context* context = (ae_renderer_context*)userData;
            float* out = (float*)outputBuffer;
            for(i=0; i<framesPerBuffer; ++i ) {
            
                
                float wave1 = sin(time*440.0f);
                float wave2 = sin(time*440.0f);
                smpl++;
                time=(float)smpl/(float)AE_SAMPLERATE;
                *out++ = wave1;
                *out++ = wave2;
            }

            if(smpl>AE_SAMPLERATE)
                smpl=smpl-AE_SAMPLERATE;
*/
            return 0;
}

void ae_init(ae_data* self) {
#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
    int err;
    err = Pa_Initialize();
    if(err!=paNoError) {
       return;
    } 

    self->context.time = 0.0f;
    err = Pa_OpenDefaultStream( &self->stream,
                                0,
                                2,
                                paFloat32,
                                AE_SAMPLERATE,
                                1024,
                                ae_renderer,
                                &self->context );
    if(err!=paNoError) {
        log_message(CTT2_RT_MODULE_AUDIO, LOG_LEVEL_ERROR, "Error:%s\n", Pa_GetErrorText( err ) );
        return;
    }

    Pa_StartStream(self->stream);
    log_message(CTT2_RT_MODULE_AUDIO, LOG_LEVEL_INFO, "Initialized audio control thread.");
#endif
    self->initialized = 1;
}

void ae_drop(ae_data* self) {
    self->initialized = 0;
#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
    if(self->stream!=0)
        Pa_StopStream(self->stream);
    Pa_Terminate();
#endif
    self->finalized = 1;
}

int ae_thread_run(void* data) {

    ae_data* self=(ae_data*) data;
    ae_init(self);
#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
    while(!self->shutdown) {

        #ifdef _WIN32
        Sleep(AUDIO_CONTROL_MS);
        #endif

        #ifdef __linux__
        usleep(AUDIO_CONTROL_MS * 1000);
        #endif

        sequencer_handle_messages();

    }
    DropSequencer();
    ae_drop(self);
#endif
    return self->finalized;
}

unsigned int initAudio() {
#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
    AE_Data.shutdown = 0;
    AE_Data.initialized = 0;
    AE_Data.stream = 0;
    InitSequencer();
    if(!SDL_CreateThread( ae_thread_run, "audio_control", &AE_Data))
        return MODULE_FAILURE;
#endif
    return MODULE_LOADED;
}

void dropAudio() {
#ifdef BEAGLE_USE_PORTAUDIO_ENGINE
    int result;
    AE_Data.shutdown = 1;
    SDL_WaitThread(ae_thread, &result);
#endif
}

/***************************************/
void audio_tracks_update_beat( int chan, void *stream, int len, void *udata) {
}

void audio_garbage_collect_channels() {
}

void audio_create_clip(audio_clip* clip, char* clip_name, double clip_beats, double clip_trigger_offset) {
}

void audio_drop_clip(audio_clip* clip) {
}

void audio_create_track(audio_track* track, double bpm, unsigned int beat_locked) {
}

void audio_play_clip_on_track(audio_clip* clip, audio_track* track, unsigned int loop) {
}

void audio_reset_tracks() {
}

void audio_track_swap_channels(audio_track* track) {
}

void audio_tick_tracks(double delta) {
}

void audio_enable_realtime_processing() {
}

void audio_disable_realtime_processing() {
}

void audio_set_volume_on_track(audio_track* track, double volume) {
}

void audio_set_track_panning(audio_track* track, double pan) {
}
