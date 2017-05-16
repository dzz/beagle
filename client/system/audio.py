import client.beagle.beagle_environment as beagle_environment
from random import choice
from math import sqrt
import audio

tracks = {}
clips = {}

class beatEngine:
    def play():
        audio.enable_beatEngine()
    def stop():
        audio.disable_beatEngine()

    def set_backdoor(idx,value):
        audio.ae_set_backdoor(idx,value)

class clip:
    def __init__(self,filename, beats = 4.0, trigger_offset = 0.0):
        print("audio.clip loading:{0}".format(filename))
        self.audio_clip = audio.clip_create(beagle_environment.get_config("app_dir") + filename, beats, trigger_offset)

    def __del__(self):
        audio.clip_drop(self.audio_clip)

class track:
    def __init__(self, bpm=128.0, beat_locked = False ):
        self.active_clip = None
        self.loop = False
        self.robin_clips = []
        self.robin_index = 0
        self.robin_mode = None
        if(beat_locked):
            bl = 1
        else:
            bl = 0
        self.audio_track = audio.track_create(bpm, bl)

    def set_robin_mode(self,mode):
        self.robin_mode = mode

    def add_robin_clip(self,clip):
        self.robin_clips.append(clip)

    def __del__(self):
        audio.track_drop(self.audio_track)

    def retrigger(self):
        if self.robin_mode == "sequence":
            self.robin_index = (self.robin_index +1) % len(self.robin_clips)
            self.active_clip = self.robin_clips[self.robin_index]
        if self.robin_mode == "random":
            self.active_clip = choice(self.robin_clips)
            
        if self.active_clip:
            self.play_clip( self.active_clip, self.loop )

    def play_clip(self,clip,loop = False):
        self.loop = loop
        self.active_clip = clip
        if(loop):
            audio.track_play_clip( self.audio_track, clip.audio_clip, 1)
        else:
            audio.track_play_clip( self.audio_track, clip.audio_clip, 0)
        return self

    def set_pan(self,pan):
        audio.track_set_pan(self.audio_track, pan)
        return self

    def set_volume(self,v):
        audio.track_set_volume(self.audio_track, v)
        return self


def get_track(name, bpm = 128.0, beatlock = False):
    global tracks
    if not name in tracks:
        tracks[name] = track(bpm, beatlock)
    return tracks[name]
    

def get_clip(name, clip_beats = 4.0, clip_trigger_offset = 0.0 ):
    global clips
    if not name in clips:
        clips[name] = clip(name, clip_beats, clip_trigger_offset)
    return clips[name]

def flush_clips():
    global clips
    clips = []


def reset_tracks():
    global tracks
    tracks = {}
    audio.track_reset_all()
    

