
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "misty.h"
}

bool setVolumeForHandle(snd_mixer_t *handle, int percentage) {
  static char card[64] = "default";
  int err = 0;

  if ((err = snd_mixer_attach(handle, card)) < 0) {
    fprintf(stderr, "Mixer attach %s error: %s", card, snd_strerror(err));
    return false;
  }

  if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
    fprintf(stderr, "Mixer register error: %s", snd_strerror(err));
    return false;
  }

  if ((err = snd_mixer_load(handle)) < 0) {
    fprintf(stderr, "Mixer %s load error: %s", card, snd_strerror(err));
    return false;
  }

  snd_mixer_selem_id_t *sid;
  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_name(sid, "PCM");
  
  snd_mixer_elem_t *element;
  if (!(element = snd_mixer_find_selem(handle, sid))) {
    fprintf(stderr, "Unable to find simple control '%s',%i\n",
            snd_mixer_selem_id_get_name(sid),
            snd_mixer_selem_id_get_index(sid));
    return false;
  }
    
  for (int i = 0; i <= SND_MIXER_SCHN_LAST; i++) {
    snd_mixer_selem_channel_id_t channel = (snd_mixer_selem_channel_id_t) i;
    if (snd_mixer_selem_has_playback_channel(element, channel)) {
      if (!snd_mixer_selem_has_playback_volume(element)) {
        fprintf(stderr, "Mixer %s does not have playback volume.", card);
        return false;
      }

      long min, max;
      if (snd_mixer_selem_get_playback_volume_range(element, &min, &max) < 0) {
        fprintf(stderr, "Mixer %s get volume range error: %s", card, snd_strerror(err));
        return false;
      }

      long val = ceil(percentage * (max - min) * 0.01 + min);

      if ((err = snd_mixer_selem_set_playback_volume(element, channel, val)) != 0) {
        fprintf(stderr, "Mixer %s set volume error: %s", card, snd_strerror(err));
        return false;
      }
    }
  }

  return true;
}

int setVolume(int percentage) {
  snd_mixer_t *handle = NULL;

  int err = 0;
  if ((err = snd_mixer_open(&handle, 0)) < 0) {
    fprintf(stderr, "Mixer open error: %s\n", snd_strerror(err));
    return false;
  }

  bool success = setVolumeForHandle(handle, percentage);

  snd_mixer_close(handle);
  return success ? 1 : 0;
}

