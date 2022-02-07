#pragma once

#include <string>

struct SimView;

void CancelRedrawView(SimView* view);
bool Eval(const std::string&);
void StopEarthquake();
void InvalidateEditors();
void InvalidateMaps();
void Kick();
void StartMicropolisTimer();
void StopMicropolisTimer();
void EventuallyRedrawView(SimView* view);
void RedrawMaps();
void DidStopPan(SimView* view);
