#pragma once

struct SimView;

void InitializeSound();
void ShutDownSound();
void MakeSound(const char* channel, const char* id);
void MakeSoundOn(SimView* view, const char* channel, const char* id);
void StartBulldozer();
void StopBulldozer();
void SoundOff();
void DoStartSound(const char* channel, const char* id);
void DoStopSound(const char* id);
