#pragma once


void DoStopMicropolis();
void UpdateFlush();
void DoTimeoutListen();
void ViewToPixelCoords(SimView* view, int x, int y, int& outx, int& outy);
void DoPanTo(SimView* view, int x, int y);
void DoPanBy(struct SimView* view, int dx, int dy);
