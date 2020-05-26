#pragma once
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

class AudioController
{
public:
    AudioController(const char* card = "default",
                    const char* selemName = "Master");
    ~AudioController() { snd_mixer_close(pHandle); }

    void setLevel(int);
    int getLevel();

private:
    snd_mixer_t* pHandle;
    snd_mixer_selem_id_t* pSid;
    snd_mixer_elem_t* pElem;
    long minVol, maxVol;
};