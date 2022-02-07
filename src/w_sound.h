#pragma once

#include <string>

struct SimView;

void InitializeSound();
void ShutDownSound();
void MakeSound(const std::string& channel, const std::string& id);
void MakeSoundOn(SimView* view, const char* channel, const char* id);
void StartBulldozer();
void StopBulldozer();
void SoundOff();
void DoStartSound(const char* channel, const char* id);
void DoStopSound(const char* id);

bool userSoundOn();
void userSoundOn(bool);
