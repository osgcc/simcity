#pragma once

struct SimView;

void CancelRedrawView(SimView* view);
int Eval(const std::string&);
void StopEarthquake();
void tk_main();
void InvalidateEditors();
void InvalidateMaps();
void Kick();
