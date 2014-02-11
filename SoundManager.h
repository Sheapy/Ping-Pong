#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_
 
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
 
class SoundManager {
    public:
        static SoundManager           SoundControl;
 
        std::vector<Mix_Chunk*>     SoundList;
 
    public:
        SoundManager();
 
        int OnLoad(char* File);
 
        void OnCleanup();
 
    public:
        void Play(int ID);
};
 
#endif
