#pragma once

#include "Point.h"
#include "w_resrc.h"

int SendMes(MessageEnumerator);
void SendMessages();
void ClearMes();
void SendMesAt(MessageEnumerator, int x, int y);
void doMessage();

MessageEnumerator MessageId();
void MessageId(MessageEnumerator);

void MessageLocation(Point<int> location);
const Point<int>& MessageLocation();
void AutoGotoMessageLocation(bool autogo);
bool AutoGotoMessageLocation();
