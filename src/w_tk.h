#pragma once

#include <string>

struct SimView;

void CancelRedrawView(SimView* view);
int Eval(const std::string&);
void StopEarthquake();
void tk_main();
void InvalidateEditors();
void InvalidateMaps();
void Kick();
void StartMicropolisTimer();
void StopMicropolisTimer();
void EventuallyRedrawView(SimView* view);
