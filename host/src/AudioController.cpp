#include "AudioController.hpp"

AudioController::AudioController(const char* card, const char* selemName)
{
    const char* pCard = card;
    const char* pSelemName = selemName;

    snd_mixer_open(&pHandle, 0);
    snd_mixer_attach(pHandle, pCard);
    snd_mixer_selem_register(pHandle, NULL, NULL);
    snd_mixer_load(pHandle);

    snd_mixer_selem_id_alloca(&pSid);
    snd_mixer_selem_id_set_index(pSid, 0);
    snd_mixer_selem_id_set_name(pSid, pSelemName);
    pElem = snd_mixer_find_selem(pHandle, pSid);

    snd_mixer_selem_get_playback_volume_range(pElem, &minVol, &maxVol);
}

void AudioController::setLevel(int volume)
{
    snd_mixer_selem_set_playback_volume_all(pElem,
        static_cast<long>(volume) * maxVol / 1023);
}

int AudioController::getLevel()
{
    long volume, range;
    double volumeAsDouble;
    snd_mixer_selem_get_playback_volume(pElem, SND_MIXER_SCHN_FRONT_LEFT, &volume);

    volume -= minVol;
    range = maxVol - minVol;
    volumeAsDouble = static_cast<double>(volume);

    if (range != 0)
        volumeAsDouble *= 100.0 / static_cast<double>(range);

    return static_cast<int>(volumeAsDouble);
}