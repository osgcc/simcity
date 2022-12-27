// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 Leeor Dicker
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
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
