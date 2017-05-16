#ifndef __HW_AUDIO__
#define __HW_AUDIO__

#include <SDL.h>
#include <SDL_mixer.h>
#include "audio/load_snd.h"



#define AE_SAMPLERATE (44100)

#define AUDIO_SAMPLERATE 32000
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNKSIZE 256
#define AUDIO_MAX_TRACKS 9
#define TICK_FILTER_A 0.75
#define BEAT_LOCKED 1

//check, approx, every X ms for control messages
#define AUDIO_CONTROL_MS 5


#define SEQUENCER_TRACKS (8)
#define SEQUENCER_CONTROL_STACK_SIZE (64)

typedef enum {
    evt_none = 0,
    evt_loop = 1
} seq_track_event_code;

typedef enum {
    msg_none = 0,
    msg_set_event = 1
} seq_track_msg_code;

typedef enum {
    beat_none       = 0,
    beat_eigth      = 1,
    beat_quarter    = 2,
    beat_half       = 3,
    beat_whole      = 4
} seq_track_beat_lock;

    

typedef struct seq_track_event {
    seq_track_event_code    evt_type;
    hw_audio_wav_data*      wav;
    unsigned int            loop_smpls;
} seq_track_event;

typedef struct {
    seq_track_msg_code      code;
    void*                   data;
    unsigned int            gcmode;  
} seq_track_msg;

typedef struct {
    seq_track_event*        active_event;
    seq_track_event*        next_event;
    seq_track_event*        old_event;
    seq_track_msg*          control_message;
    unsigned int            cur_sample;
    seq_track_beat_lock     beat_lock;
    unsigned int            beat_lock_mod;
} seq_track;

typedef struct {
    seq_track*              tracks;
    unsigned int            n_tracks;
    unsigned int            bpm;
    unsigned int            halt;
} seq_sequencer;


unsigned int initAudio();
void dropAudio();

void sequencer_issue_message( unsigned int track, seq_track_msg* message );

typedef struct {
    Mix_Chunk* ChunkData;
    double clip_beats;
    double clip_trigger_offset;
} audio_clip;

typedef struct {
    int master_track_channel;
    int slave_track_channel;
    int active_track_channel;
    unsigned int beat_locked;
    double volume_filtered;
    double volume_set;
    double bpm;
    double bpsec;
    double bpsmp;
    double beat;
    double bpt;
    double next_beat_trigger;
    audio_clip* next_clip;
    audio_clip* active_clip;
    unsigned int next_clip_loops;
} audio_track;

void audio_create_clip(audio_clip* clip, char* clip_name, double clip_beats, double clip_trigger_offset);
void audio_drop_clip(audio_clip* clip);
void audio_create_track(audio_track* track, double bpm, unsigned int beat_locked);
void audio_reset_tracks();
void audio_play_clip_on_track(audio_clip* clip, audio_track* track, unsigned int loop);
void audio_set_volume_on_track(audio_track* track, double volume);
void audio_set_track_panning(audio_track* track, double pan);
void audio_tick_tracks(double delta);
void audio_enable_realtime_processing();
void audio_disable_realtime_processing();
void audio_garbage_collect_channels();
void ae_set_backdoor(unsigned int idx, float value);
void sequencer_queue_wav( unsigned int track, hw_audio_wav_data* wav);
void sequencer_halt();
#endif

