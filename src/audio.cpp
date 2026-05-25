#include "audio.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>

#include <cstdlib>
#include <string>

static const char* SoundPath(GameSound sound)
{
    switch (sound)
    {
        case GameSound::Shotgun:
            return "../../data/sounds/shotgun_sound.mp3";
        case GameSound::CollectDrink:
            return "../../data/sounds/collect_energy_drinkmp3.mp3";
        case GameSound::Footstep:
            return "../../data/sounds/footstep.mp3";
        case GameSound::BigfootKillsPlayer:
            return "../../data/sounds/bigfoot_kills_player.mp3";
        case GameSound::BigfootDies:
            return "../../data/sounds/bigfoot_dies.mp3";
    }

    return "";
}

static void PlayMp3File(const char* path, const char* alias, int volume = 1000)
{
    std::string close_command = std::string("close ") + alias;
    mciSendStringA(close_command.c_str(), NULL, 0, NULL);

    std::string open_command = std::string("open \"") + path + "\" type mpegvideo alias " + alias;

    if (mciSendStringA(open_command.c_str(), NULL, 0, NULL) != 0)
        return;

    std::string volume_command = std::string("setaudio ") + alias + " volume to " + std::to_string(volume);
    mciSendStringA(volume_command.c_str(), NULL, 0, NULL);

    std::string play_command = std::string("play ") + alias + " from 0";
    mciSendStringA(play_command.c_str(), NULL, 0, NULL);
}

void PlayGameSound(GameSound sound)
{
    switch (sound)
    {
        case GameSound::Shotgun:
            PlayMp3File(SoundPath(sound), "sound_shotgun");
            break;
        case GameSound::CollectDrink:
            PlayMp3File(SoundPath(sound), "sound_collect_0");
            PlayMp3File(SoundPath(sound), "sound_collect_1");
            PlayMp3File(SoundPath(sound), "sound_collect_2");
            PlayMp3File(SoundPath(sound), "sound_collect_3");
            break;
        case GameSound::Footstep:
            PlayMp3File(SoundPath(sound), "sound_footstep", 130);
            break;
        case GameSound::BigfootKillsPlayer:
            PlayMp3File(SoundPath(sound), "sound_bigfoot_kill");
            break;
        case GameSound::BigfootDies:
            PlayMp3File(SoundPath(sound), "sound_bigfoot_die");
            break;
    }
}

void PlayRandomBigfootRoar()
{
    const char* alias = "sound_bigfoot_roar";
    static const char* roar_paths[] = {
        "../../data/sounds/bigfoot_roars_multiple_1.mp3",
        "../../data/sounds/bigfoot_roars_multiple_2.mp3",
        "../../data/sounds/bigfoot_roars_multiple_3.mp3",
        "../../data/sounds/bigfoot_roars_multiple_4.mp3",
        "../../data/sounds/bigfoot_roars_multiple_5.mp3",
        "../../data/sounds/bigfoot_roars_multiple_6.mp3",
        "../../data/sounds/bigfoot_roars_multiple_7.mp3",
        "../../data/sounds/bigfoot_roars_multiple_8.mp3",
        "../../data/sounds/bigfoot_roars_multiple_9.mp3"
    };
    const int roar_count = (int)(sizeof(roar_paths) / sizeof(roar_paths[0]));
    const char* path = roar_paths[rand() % roar_count];

    mciSendStringA("close sound_bigfoot_roar", NULL, 0, NULL);

    std::string open_command = std::string("open \"") + path + "\" type mpegvideo alias " + alias;

    if (mciSendStringA(open_command.c_str(), NULL, 0, NULL) != 0)
        return;

    mciSendStringA("play sound_bigfoot_roar from 0", NULL, 0, NULL);
}

void StartBackgroundMusic()
{
    const char* alias = "music_background";
    const char* path = "../../data/sounds/Where_the_Lights_Fail.mp3";

    mciSendStringA("close music_background", NULL, 0, NULL);

    std::string open_command = std::string("open \"") + path + "\" type mpegvideo alias " + alias;

    if (mciSendStringA(open_command.c_str(), NULL, 0, NULL) != 0)
        return;

    mciSendStringA("setaudio music_background volume to 100", NULL, 0, NULL);
    mciSendStringA("play music_background from 0 repeat", NULL, 0, NULL);
}

void StopBackgroundMusic()
{
    mciSendStringA("stop music_background", NULL, 0, NULL);
    mciSendStringA("close music_background", NULL, 0, NULL);
}

#else

void PlayGameSound(GameSound)
{
}

void PlayRandomBigfootRoar()
{
}

void StartBackgroundMusic()
{
}

void StopBackgroundMusic()
{
}

#endif
