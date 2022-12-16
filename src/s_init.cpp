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

#include "Evaluation.h"
#include "Sprite.h"
#include "Tool.h"

#include "s_alloc.h"
#include "s_msg.h"
#include "s_sim.h"

#include "w_update.h"
#include "w_util.h"


void InitWillStuff()
{
    RoadEffect = 32;
    PoliceEffect = 1000;
    FireEffect = 1000;
    cityScore(500);
    cityPopulation(-1);
    LastCityTime(-1);
    LastCityYear(1);
    LastCityMonth(0);
    pendingTool(Tool::None);

    MessageId(NotificationId::None);

    destroyAllSprites();

    DisasterEvent = 0;

    initMapArrays();

    DoNewGame();
}
