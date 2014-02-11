#include "SoundManager.h"
 
SoundManager SoundManager::SoundControl;
 
SoundManager::SoundManager() {
}
 
int SoundManager::OnLoad(char* File) {
    Mix_Chunk* TempSound = NULL;
 
    if((TempSound = Mix_LoadWAV(File)) == NULL) {
        return -1;
    }
 
    SoundList.push_back(TempSound);
 
    return (SoundList.size() - 1);
}
 
void SoundManager::OnCleanup() {
    for(int i = 0;i < SoundList.size();i++) {
        Mix_FreeChunk(SoundList[i]);
    }
 
    SoundList.clear();
}
 
void SoundManager::Play(int ID) {
    if(ID < 0 || ID >= SoundList.size()) return;
    if(SoundList[ID] == NULL) return;
 
    Mix_PlayChannel(-1, SoundList[ID], 0);
}
