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

#include "Map.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_sim.h"

#include "w_sound.h"
#include "Sprite.h"
#include "w_stubs.h"
#include "w_tk.h"
#include "w_tool.h"


static char LastKeys[5];


/* TODO:
  <n> new city
  <l> load city
  <s> save city
  <q> quit
  <a> autobulldoze
  <0><1><2><3>... game speed
  <m> map window
  <g> graph window
  <b> budget window
  <e> evaluation window
*/


void ResetLastKeys()
{
    LastKeys[0] = ' ';
    LastKeys[1] = ' ';
    LastKeys[2] = ' ';
    LastKeys[3] = ' ';
    LastKeys[4] = '\0';
    PunishCnt = 0;
}


/* comefrom: processEvent */
void doKeyDown(SimView* view, int charCode)
{
    LastKeys[0] = LastKeys[1];
    LastKeys[1] = LastKeys[2];
    LastKeys[2] = LastKeys[3];
    LastKeys[3] = tolower(charCode);

    if (strcmp(LastKeys, "fund") == 0) {
        Spend(-10000);
        PunishCnt++;				/* punish for cheating */
        if (PunishCnt == 5) {
            PunishCnt = 0;
            MakeEarthquake();
        }
        LastKeys[0] = '\0';
    }
    else if (strcmp(LastKeys, "fart") == 0) {
        MakeSound("city", "Explosion-High");
        MakeSound("city", "Explosion-Low");
        MakeFire();
        MakeFlood();
        MakeTornado();
        MakeEarthquake();
        MakeMonster();
        LastKeys[0] = '\0';
    }
    else if (strcmp(LastKeys, "nuke") == 0) {
        int i, j;
        MakeSound("city", "Explosion-High");
        MakeSound("city", "Explosion-Low");
        for (i = 0; i < SimWidth; i++) {
            for (j = 0; j < SimHeight; j++) {
                int tile = Map[i][j] & LOMASK;
                if ((tile >= RUBBLE) &&
                    ((tile < CHURCH - 4) ||
                        (tile > CHURCH + 4))) {
                    if ((tile >= HBRIDGE && tile <= VBRIDGE) ||
                        (tile >= BRWH && tile <= LTRFBASE + 1) ||
                        (tile >= BRWV && tile <= BRWV + 2) ||
                        (tile >= BRWXXX1 && tile <= BRWXXX1 + 2) ||
                        (tile >= BRWXXX2 && tile <= BRWXXX2 + 2) ||
                        (tile >= BRWXXX3 && tile <= BRWXXX3 + 2) ||
                        (tile >= BRWXXX4 && tile <= BRWXXX4 + 2) ||
                        (tile >= BRWXXX5 && tile <= BRWXXX5 + 2) ||
                        (tile >= BRWXXX6 && tile <= BRWXXX6 + 2) ||
                        (tile >= BRWXXX7 && tile <= BRWXXX7 + 2)) {
                        Map[i][j] = RIVER;
                    }
                    else {
                        Map[i][j] = TINYEXP + ANIMBIT + BULLBIT + RandomRange(0, 2);
                    }
                }
            }
        }
        LastKeys[0] = '\0';
    }
    else if (strcmp(LastKeys, "olpc") == 0) {
        Spend(-1000000);
    }

    switch (charCode) {

        /*
      case 'X':
      case 'x': {
        int s = view->tool_state;
        if (++s > lastState) {
      s = firstState;
        }
        setWandState(s);
        break;
      }

      case 'Z':
      case 'z': {
        int s = view->tool_state;
        if (--s < firstState) {
        s = lastState;
        }
        setWandState(s);
        break;
      }
      */

      /***** shift wand state to bull dozer *****/
    case 'B':
    case 'b':
    case 'B' - '@':
    {
        //if (view->tool_state_save == -1)
        //{
        //    view->tool_state_save = view->tool_state;
        //}
        //setWandState(Tool::Bulldoze);
        break;
    }

    /***** shift wand state to roads *****/
    case 'R':
    case 'r':
    case 'R' - '@':
    {
        //if (view->tool_state_save == -1) {
        //view->tool_state_save = view->tool_state;
        //}
        //setWandState(Tool::Road);
        break;
    }

    /***** shift wand state to power *****/
    case 'P':
    case 'p':
    case 'P' - '@':
    {
        //if (view->tool_state_save == -1)
        //{
        //    view->tool_state_save = view->tool_state;
        //}
        //setWandState(Tool::Wire);
        break;
    }

    /***** shift wand state to transit *****/
    case 'T':
    case 't':
    case 'T' - '@':
    {
        //if (view->tool_state_save == -1)
        //{
        //    view->tool_state_save = view->tool_state;
        //}
        //setWandState(Tool::Rail);
        break;
    }

    case 27:
        SoundOff();
        break;

    }
}


void doKeyUp(SimView* view, int charCode)
{
    switch (charCode)
    {
    case 'b':
    case 'B':
    case 'B' - '@':
    case 'R':
    case 'r':
    case 'R' - '@':
    case 'P':
    case 'p':
    case 'P' - '@':
    case 'T':
    case 't':
    case 'T' - '@':
    case 'q':
    case 'Q':
    case 'Q' - '@':
        /*
        if (view->tool_state_save != -1)
        {
            setWandState(view->tool_state_save);
        }
        view->tool_state_save = -1;
        */
        break;

    default:
        break;
    }
}
