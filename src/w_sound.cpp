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
#include "main.h"
#include "w_tk.h"

#include <string>


/* Sound routines */


static bool SoundInitialized = false;
static bool UserSoundOn = true;
int Dozing = 0;


bool userSoundOn()
{
    return SoundInitialized;
}

void userSoundOn(bool val)
{
    SoundInitialized = val;
}



void InitializeSound()
{
    SoundInitialized = true;

    // load sound samples here
}


void ShutDownSound()
{
    if (SoundInitialized)
    {
        // unload sound samples here
    }
}


void MakeSound(const std::string& channel, const std::string& id)
{
    if (!UserSoundOn)
    {
        return;
    }
    if (!SoundInitialized)
    {
        InitializeSound();
    }

    Eval("UIMakeSound \"" + channel + "\" \"" + id + "\"");
}


void MakeSoundOn(SimView* view, const char* channel, const char* id)
{
    char buf[256]{};

    if (!UserSoundOn)
    {
        return;
    }
    if (!SoundInitialized)
    {
        InitializeSound();
    }

    //sprintf(buf, "UIMakeSoundOn %s \"%s\" \"%s\"", /*Tk_PathName(view->tkwin)*/ "window-path", channel, id);
    Eval(buf);
}


void SoundOff()
{
    if (!SoundInitialized)
    {
        InitializeSound();
    }
    Eval("UISoundOff");
    Dozing = 0;
}


void DoStartSound(const std::string& channel, const std::string& id)
{
    Eval("UIStartSound " + channel + " " + id);
}


void DoStopSound(const char* id)
{
    Eval("UIStopSound " + std::string(id));
}


void StartBulldozer()
{
    if (!UserSoundOn)
    {
        return;
    }
    if (!SoundInitialized)
    {
        InitializeSound();
    }
    if (!Dozing)
    {
        DoStartSound("edit", "1");
        Dozing = 1;
    }
}


void StopBulldozer()
{
    if ((!UserSoundOn) || (!SoundInitialized))
    {
        return;
    }

    DoStopSound("1");
    Dozing = 0;
}
