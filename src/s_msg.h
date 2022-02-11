#pragma once

#include "Point.h"

int SendMes(int);
void SendMessages();
void ClearMes();
void SendMesAt(int Mnum, int x, int y);
void doMessage();

int MessageId();
void MessageId(int id);

void MessageLocation(Point<int> location);
const Point<int>& MessageLocation();
