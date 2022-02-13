#pragma once

#include "Point.h"
#include "w_resrc.h"

int SendMes(NotificationId);
void SendMessages();
void ClearMes();
void SendMesAt(NotificationId, int x, int y);
void doMessage();

NotificationId MessageId();
void MessageId(NotificationId);

void MessageLocation(Point<int> location);
const Point<int>& MessageLocation();
void AutoGotoMessageLocation(bool autogo);
bool AutoGotoMessageLocation();
const std::string& LastMessage();
void MessageDisplayTime(int time);
int MessageDisplayTime();
