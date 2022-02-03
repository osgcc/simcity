#pragma once

struct SimView;

void CancelRedrawView(SimView* view);
int Eval(const char* buf);
void StopEarthquake();
void tk_main();
void InvalidateEditors();
void InvalidateMaps();
