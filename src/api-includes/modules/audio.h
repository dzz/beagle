/* ~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~
 * import audio
 * =============
 */

MODULE_FUNC baudy_load_sound 
DEF_ARGS {
    char *filename;
    if(!INPUT_ARGS(args,"s",&filename))
        return NULL;

    printf(filename);
    printf("\n");

    unsigned int sound_id = BGLBasicMixer_LoadSound(filename);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)sound_id);
}

MODULE_FUNC baudy_play_music
DEF_ARGS {
    char *filename;
    if(!INPUT_ARGS(args,"s",&filename))
        return NULL;

    printf(filename);
    printf("\n");

    BGLBasicMixer_PlayMusic(filename);
    Py_RETURN_NONE;
}

MODULE_FUNC baudy_stop_music
DEF_ARGS {
    BGLBasicMixer_StopMusic();
    Py_RETURN_NONE;
}

MODULE_FUNC baudy_play_sound 
DEF_ARGS {
    unsigned int sound_id;
    if(!INPUT_ARGS(args,"I",&sound_id))
        return NULL;
    BGLBasicMixer_PlaySound(sound_id);
    Py_RETURN_NONE;
}

MODULE_FUNC audio_clip_create
DEF_ARGS {
    /*
    audio_clip* clip;
    char *filename;
    float clip_beats;
    float clip_trigger_offset;
    clip = malloc(sizeof(audio_clip));
    if(!INPUT_ARGS(args,"sff",&filename, &clip_beats, &clip_trigger_offset))
        return NULL;
    audio_create_clip(clip,filename, (double)clip_beats, (double)clip_trigger_offset);
    return Py_BuildValue("I",(unsigned int)clip);
    */

    hw_audio_wav_data* wav;
    char *filename;
    wav = (hw_audio_wav_data*) malloc(sizeof(hw_audio_wav_data));
    if(!INPUT_ARGS(args,"s",&filename))
        return NULL;
    audio_load_wav(wav, filename);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)wav);
}

MODULE_FUNC audio_clip_drop
DEF_ARGS {
    marshalled_pointer ptr;
    hw_audio_wav_data* wav;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr))
        return NULL;
    wav = (hw_audio_wav_data*)ptr;
    audio_drop_wav(wav);
    free(wav);
    Py_RETURN_NONE;
}

MODULE_FUNC audio_track_create
DEF_ARGS {
    audio_track* track;
    float bpm;
    unsigned int beatlock;
    track = malloc(sizeof(audio_track));
    if(!INPUT_ARGS(args,"fI",&bpm,&beatlock))
        return NULL;
    audio_create_track(track, bpm, beatlock);
    return Py_BuildValue(PYTHON_POINTER_INT,(marshalled_pointer)track);
}

MODULE_FUNC audio_track_drop
DEF_ARGS {
    marshalled_pointer ptr;
    audio_track* track;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT,&ptr))
        return NULL;
    track = (audio_track*)ptr;
    free(track);
    Py_RETURN_NONE;
}

MODULE_FUNC audio_track_reset_all
DEF_ARGS {
    audio_reset_tracks();
    Py_RETURN_NONE;
}

MODULE_FUNC audio_track_play_clip
DEF_ARGS {
    unsigned int track_id;
    hw_audio_wav_data* wav;

    if(!INPUT_ARGS(args,"I" PYTHON_POINTER_INT,&track_id,&wav))  {
        return NULL;
    }

    sequencer_queue_wav( track_id, wav);
    Py_RETURN_NONE;
}

MODULE_FUNC audio_track_set_volume
DEF_ARGS {
    marshalled_pointer tptr;
    float volume;
    audio_track* track;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "f",&tptr,&volume)) 
        return NULL;

    track = (audio_track*)tptr;
    audio_set_volume_on_track(track,(double)volume);
    Py_RETURN_NONE;
}

MODULE_FUNC audio_track_set_pan
DEF_ARGS {
    marshalled_pointer tptr;
    float pan;
    audio_track* track;
    if(!INPUT_ARGS(args,PYTHON_POINTER_INT "f",&tptr,&pan)) 
        return NULL;

    track = (audio_track*)tptr;
    audio_set_track_panning(track,(double)pan);
    Py_RETURN_NONE;
}

MODULE_FUNC audio_enable_beatEngine
DEF_ARGS {
    audio_enable_realtime_processing();
    Py_RETURN_NONE;
}

MODULE_FUNC audio_disable_beatEngine
DEF_ARGS {
    audio_enable_realtime_processing();
    Py_RETURN_NONE;
}

MODULE_FUNC audio_ae_set_backdoor
DEF_ARGS {
    unsigned int backdoor;
    float value;
    if(!INPUT_ARGS(args,"If",&backdoor, &value))
       return NULL;
    ae_set_backdoor(backdoor, value);
    Py_RETURN_NONE;
}
/*~=`=`=`=`=`=`=`=`=`=`==`=`=`=`=`=`=`=`=`=`=`=`=``=`=`=`=`=`=`=`=`=`=`=`=`=*/
static PyMethodDef audio_methods[] = {
    {"clip_create",                     audio_clip_create,                  METH_VARARGS, NULL},
    {"clip_drop",                       audio_clip_drop,                    METH_VARARGS, NULL},
    {"track_create",                    audio_track_create,                 METH_VARARGS, NULL},
    {"track_drop",                      audio_track_drop,                   METH_VARARGS, NULL},
    {"track_reset_all",                 audio_track_reset_all,              METH_VARARGS, NULL},
    {"track_play_clip",                 audio_track_play_clip,              METH_VARARGS, NULL},
    {"track_set_volume",                audio_track_set_volume,             METH_VARARGS, NULL},
    {"track_set_pan",                   audio_track_set_pan,                METH_VARARGS, NULL},
    {"enable_beatEngine",       audio_enable_beatEngine,   METH_VARARGS, NULL},
    {"disable_beatEngine",      audio_disable_beatEngine,  METH_VARARGS, NULL},
    {"ae_set_backdoor",      audio_ae_set_backdoor,  METH_VARARGS, NULL},
    {"baudy_load_sound",      baudy_load_sound,  METH_VARARGS, NULL},
    {"baudy_play_sound",      baudy_play_sound,  METH_VARARGS, NULL},
    {"baudy_play_music",      baudy_play_music,  METH_VARARGS, NULL},
    {"baudy_stop_music",      baudy_stop_music,  METH_VARARGS, NULL},
    {NULL,NULL,0,NULL } /*terminator record*/
};

static PyModuleDef audio_module = {
    PyModuleDef_HEAD_INIT, "audio", NULL, -1, audio_methods,
    NULL,NULL,NULL,NULL
};

static PyObject* PyInit_audio(void) {
    return PyModule_Create(&audio_module);
}
