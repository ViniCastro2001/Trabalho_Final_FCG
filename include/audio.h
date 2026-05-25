#ifndef AUDIO_H
#define AUDIO_H

enum class GameSound
{
    Shotgun,
    CollectDrink,
    Footstep,
    BigfootKillsPlayer,
    BigfootDies
};

void PlayGameSound(GameSound sound);
void PlayRandomBigfootRoar();
void StartBackgroundMusic();
void StopBackgroundMusic();

#endif
