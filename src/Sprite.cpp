/* w_sprite.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

#include "Sprite.h"

#include "main.h"

#include "Map.h"

#include "s_alloc.h"
#include "s_msg.h"
#include "s_sim.h"

#include "w_sound.h"
#include "w_tool.h"
#include "w_util.h"

#include <string>

#include <SDL2/SDL.h>


int CrashX, CrashY;
int absDist;
int Cycle;

std::vector<SimSprite> Sprites;


void GetObjectXpms(int id, int frames, std::vector<Texture>& frameList)
{
    std::string name;

    for (int i = 0; i < frames; i++)
    {
        name = std::string("images/obj") + std::to_string(id) + "-" + std::to_string(i) + ".xpm";
        frameList.push_back(loadTexture(MainWindowRenderer, name));
    }
}


void InitSprite(SimSprite& sprite, int x, int y)
{
    sprite.x = x;
    sprite.y = y;
    sprite.frame = 0;
    sprite.orig_x = 0;
    sprite.orig_y = 0;
    sprite.dest_x = 0;
    sprite.dest_y = 0;
    sprite.count = 0;
    sprite.sound_count = 0;
    sprite.dir = 0;
    sprite.new_dir = 0;
    sprite.step = 0;
    sprite.flag = 0;
    sprite.control = -1;
    sprite.turn = 0;
    sprite.accel = 0;
    sprite.speed = 100;
    sprite.active = true;

    /*
    pm[0] = NULL; // no object number 0
    pm[TRA] = GetObjectXpms(TRA, 5);
    pm[COP] = GetObjectXpms(COP, 8);
    pm[AIR] = GetObjectXpms(AIR, 11);
    pm[SHI] = GetObjectXpms(SHI, 8);
    pm[GOD] = GetObjectXpms(GOD, 16);
    pm[TOR] = GetObjectXpms(TOR, 3);
    pm[EXP] = GetObjectXpms(EXP, 6);
    pm[BUS] = GetObjectXpms(BUS, 4);
    */

    switch (sprite.type)
    {

    case TRA:
        sprite.width = 32;
        sprite.height = 32;
        sprite.x_offset = 32;
        sprite.y_offset = -16;
        sprite.x_hot = 40;
        sprite.y_hot = -8;
        sprite.frame = 1;
        sprite.dir = 4;
        GetObjectXpms(TRA, 5, sprite.frames);
        break;

    case SHI:
        sprite.width = sprite.height = 48;
        sprite.x_offset = 32; sprite.y_offset = -16;
        sprite.x_hot = 48; sprite.y_hot = 0;

        if (x < (4 << 4))
        {
            sprite.frame = 3;
        }
        else if (x >= ((SimWidth - 4) << 4))
        {
            sprite.frame = 7;
        }
        else if (y < (4 << 4))
        {
            sprite.frame = 5;
        }
        else if (y >= ((SimHeight - 4) << 4))
        {
            sprite.frame = 1;
        }
        else
        {
            sprite.frame = 3;
        }

        sprite.new_dir = sprite.frame;
        sprite.dir = 10;
        sprite.count = 1;
        break;

    case GOD:
        sprite.width = sprite.height = 48;
        sprite.x_offset = 24; sprite.y_offset = 0;
        sprite.x_hot = 40; sprite.y_hot = 16;
        if (x > ((SimWidth << 4) / 2))
        {
            if (y > ((SimHeight << 4) / 2)) sprite.frame = 10;
            else sprite.frame = 7;
        }
        else if (y > ((SimHeight << 4) / 2))
        {
            sprite.frame = 1;
        }
        else
        {
            sprite.frame = 4;
        }
        sprite.count = 1000;
        sprite.dest_x = PolMaxX << 4;
        sprite.dest_y = PolMaxY << 4;
        sprite.orig_x = sprite.x;
        sprite.orig_y = sprite.y;
        break;

    case COP:
        sprite.width = sprite.height = 32;
        sprite.x_offset = 32; sprite.y_offset = -16;
        sprite.x_hot = 40; sprite.y_hot = -8;
        sprite.frame = 5;
        sprite.count = 1500;
        sprite.dest_x = RandomRange(0, SimWidth - 1);
        sprite.dest_y = RandomRange(0, SimHeight - 1);
        sprite.orig_x = x - 30;
        sprite.orig_y = y;
        break;

    case AIR:
        sprite.width = sprite.height = 48;
        sprite.x_offset = 24; sprite.y_offset = 0;
        sprite.x_hot = 48; sprite.y_hot = 16;
        if (x > ((SimWidth - 20) << 4))
        {
            sprite.x -= 100 + 48;
            sprite.dest_x = sprite.x - 200;
            sprite.frame = 7;
        }
        else
        {
            sprite.dest_x = sprite.x + 200;
            sprite.frame = 11;
        }
        sprite.dest_y = sprite.y;
        break;

    case TOR:
        sprite.width = sprite.height = 48;
        sprite.x_offset = 24; sprite.y_offset = 0;
        sprite.x_hot = 40; sprite.y_hot = 36;
        sprite.frame = 0;
        sprite.count = 200;
        GetObjectXpms(TOR, 3, sprite.frames);
        break;

    case EXP:
        sprite.width = sprite.height = 48;
        sprite.x_offset = 24; sprite.y_offset = 0;
        sprite.x_hot = 40; sprite.y_hot = 16;
        sprite.frame = 1;
        break;

    case BUS:
        sprite.width = sprite.height = 32;
        sprite.x_offset = 30; sprite.y_offset = -18;
        sprite.x_hot = 40; sprite.y_hot = -8;
        sprite.frame = 1;
        sprite.dir = 1;
        break;

    }
}


void DestroyAllSprites()
{
    Sprites.clear();
}


void DestroySprite(SimSprite& sprite)
{
    for (size_t i = 0; i < Sprites.size(); ++i)
    {
        auto& existing = Sprites[i];
        if (existing.type == sprite.type)
        {
            Sprites.erase(Sprites.begin() + i);
            return;
        }
    }
}


SimSprite* GetSprite(int type)
{

    for (size_t i = 0; i < Sprites.size(); ++i)
    {
        auto& sprite = Sprites[i];
        if (sprite.type == type)
        {
            return &sprite;
        }
    }

    return nullptr;
}


void MakeSprite(int type, int x, int y)
{
    for (size_t i = 0; i < Sprites.size(); ++i)
    {
        auto& sprite = Sprites[i];
        if (sprite.type == type)
        {
            sprite.active = true;
            sprite.x = x;
            sprite.y = y;
            return;
        }
    }

    Sprites.push_back({});
    Sprites.back().type = type;
    InitSprite(Sprites.back(), x, y);
}


void DrawSprite(SimSprite& sprite)
{
    if(!sprite.active)
    {
        return;
    }

    const auto& spriteFrame = sprite.frames[sprite.frame];

    const SDL_Rect dstRect
    {
        sprite.x - viewOffset().x + sprite.x_offset,
        sprite.y - viewOffset().y + sprite.y_offset,
        spriteFrame.dimensions.x,
        spriteFrame.dimensions.y
    };

    SDL_RenderCopy(MainWindowRenderer, spriteFrame.texture, &spriteFrame.area, &dstRect);
}


void DrawObjects()
{
    for (size_t i = 0; i < Sprites.size(); ++i)
    {
        DrawSprite(Sprites[i]);
    }
}


int GetChar(int x, int y)
{
  x >>= 4;
  y >>= 4;
  if (!CoordinatesValid(x, y, SimWidth, SimHeight))
    return(-1);
  else
    return(Map[x][y] & LOMASK);
}


int TurnTo(int p, int d)
{
  if (p == d) return(p);
  if (p < d)
    if ((d - p) < 4) p++;
    else p--;
  else
    if ((p - d) < 4) p--;
    else p++;
  if (p > 8) p = 1;
  if (p < 1) p = 8;
  return(p);
}


bool TryOther(int Tpoo, int Told, int Tnew)
{
    int z;

    z = Told + 4;
    if (z > 8)
    {
        z -= 8;
    }

    if (Tnew != z)
    {
        return(0);
    }

    if ((Tpoo == POWERBASE) || (Tpoo == POWERBASE + 1) || (Tpoo == RAILBASE) || (Tpoo == RAILBASE + 1))
    {
        return true;
    }

    return false;
}


int SpriteNotInBounds(SimSprite *sprite)
{
  int x = sprite->x + sprite->x_hot;
  int y = sprite->y + sprite->y_hot;

  if ((x < 0) || (y < 0) ||
      (x >= (SimWidth <<4)) ||
      (y >= (SimHeight <<4))) {
    return (1);
  }
  return (0);
}


int GetDir(int orgX, int orgY, int desX, int desY)
{
  static int Gdtab[13] = { 0, 3, 2, 1, 3, 4, 5, 7, 6, 5, 7, 8, 1 };
  int dispX, dispY, z;

  dispX = desX - orgX;
  dispY = desY - orgY;
  if (dispX < 0)
    if (dispY < 0) z = 11;
    else z = 8;
  else
    if (dispY < 0) z = 2;
    else z = 5;
  if (dispX < 0) dispX = -dispX;
  if (dispY < 0) dispY = -dispY;

  absDist = dispX + dispY;

  if ((dispX <<1) < dispY) z++;
  else if ((dispY <<1) < dispY) z--;

  if ((z < 0) || (z > 12)) z = 0;

  return (Gdtab[z]);
}


int GetDis(int x1, int y1, int x2, int y2)
{
    int dispX, dispY;

    if (x1 > x2)
    {
        dispX = x1 - x2;
    }
    else
    {
        dispX = x2 - x1;
    }
 
    if (y1 > y2)
    {
        dispY = y1 - y2;
    }
    else
    {
        dispY = y2 - y1;
    }

    return (dispX + dispY);
}


int CanDriveOn(int x, int y)
{
    int tile;

    if (!CoordinatesValid(x, y, SimWidth, SimHeight))
    {
        return 0;
    }

    tile = Map[x][y] & LOMASK;

    if (((tile >= ROADBASE) &&
        (tile <= LASTROAD) &&
        (tile != BRWH) &&
        (tile != BRWV)) ||
        (tile == HRAILROAD) ||
        (tile == VRAILROAD))
    {
        return 1;
    }

    if ((tile == DIRT) || tally(tile))
    {
        return -1;
    }

    return 0;
}


bool CheckSpriteCollision(SimSprite* s1, SimSprite* s2)
{
    return ((s1->frame != 0) && (s2->frame != 0) &&
        GetDis(s1->x + s1->x_hot, s1->y + s1->y_hot, s2->x + s2->x_hot, s2->y + s2->y_hot) < 30);
}


bool checkWet(int x)
{
    return ((x == POWERBASE) ||
        (x == POWERBASE + 1) ||
        (x == RAILBASE) ||
        (x == RAILBASE + 1) ||
        (x == BRWH) ||
        (x == BRWV));
}


void OFireZone(int Xloc, int Yloc, int ch)
{
    int XYmax;

    RateOGMem[Xloc >> 3][Yloc >> 3] -= 20;

    ch &= LOMASK;
    if (ch < PORTBASE)
    {
        XYmax = 2;
    }
    else
    {
        if (ch == AIRPORT)
        {
            XYmax = 5;
        }
        else
        {
            XYmax = 4;
        }
    }

    for (int x = -1; x < XYmax; x++)
    {
        for (int y = -1; y < XYmax; y++)
        {
            const int Xtem = Xloc + x;
            const int Ytem = Yloc + y;
            if ((Map[Xtem][Ytem] & LOMASK) >= ROADBASE)
            {
                Map[Xtem][Ytem] |= BULLBIT;
            }
        }
    }
}


void StartFire(int x, int y)
{
    // egad, modifying parameters
    x >>= 4;
    y >>= 4;
    if ((x >= SimWidth) || (y >= SimHeight) || (x < 0) || (y < 0))
    {
        return;
    }

    const int z = Map[x][y];
    const int t = z & LOMASK;

    if ((!(z & BURNBIT)) && (t != 0))
    {
        return;
    }

    if (z & ZONEBIT)
    {
        return;
    }

    Map[x][y] = FIRE + (Rand16() & 3) + ANIMBIT;
}


void Destroy(int ox, int oy)
{
    int x = ox >> 4; // ox == offset? Origin?
    int y = oy >> 4;

    if (!CoordinatesValid(x, y, SimWidth, SimHeight))
    {
        return;
    }

    int z = Map[x][y];
    int t = z & LOMASK;

    if (t >= TREEBASE)
    {
        /* TILE_IS_BRIDGE(t) */
        if (!(z & BURNBIT))
        {
            if ((t >= ROADBASE) && (t <= LASTROAD))
            {
                Map[x][y] = RIVER;
                return;
            }
        }
        if (z & ZONEBIT)
        {
            OFireZone(x, y, z);
            if (t > RZB)
            {
                MakeExplosionAt(ox, oy);
            }
        }
        if (checkWet(t))
        {
            Map[x][y] = RIVER;
        }
        else
        {
            Map[x][y] = (DoAnimation ? TINYEXP : (LASTTINYEXP - 3)) | BULLBIT | ANIMBIT;
        }
    }
}


void ExplodeSprite(SimSprite* sprite)
{
    int x, y;

    sprite->frame = 0;

    x = sprite->x + sprite->x_hot;
    y = sprite->y + sprite->y_hot;
    MakeExplosionAt(x, y);

    x = (x >> 4);
    y = (y >> 4);

    switch (sprite->type)
    {
    case AIR:
        CrashX = x;
        CrashY = y;
        SendMesAt(NotificationId::PlaneCrashed, x, y);
        break;

    case SHI:
        CrashX = x;
        CrashY = y;
        SendMesAt(NotificationId::ShipWrecked, x, y);
        break;

    case TRA:
        CrashX = x;
        CrashY = y;
        SendMesAt(NotificationId::TrainCrashed, x, y);
        break;

    case COP:
        CrashX = x;
        CrashY = y;
        SendMesAt(NotificationId::HelicopterCrashed, x, y);
        break;

    case BUS:
        CrashX = x;
        CrashY = y;
        SendMesAt(NotificationId::TrainCrashed, x, y); /* XXX for now */
        break;
    }

    MakeSound("city", "Explosion-High"); /* explosion */
    return;
}


void DoTrainSprite(SimSprite& sprite)
{
    static int Cx[4] = { 0,  16,   0, -16 };
    static int Cy[4] = { -16,   0,  16,   0 };
    static int Dx[5] = { 0,   4,   0,  -4,   0 };
    static int Dy[5] = { -4,   0,   4,   0,   0 };

    static int TrainPic2[5] = { 0, 1, 0, 1, 4 };

    if ((sprite.frame == 3) || (sprite.frame == 4))
    {
        sprite.frame = TrainPic2[sprite.dir];
    }

    sprite.x += Dx[sprite.dir];
    sprite.y += Dy[sprite.dir];

    int dir = RandomRange(0, 4);
    for (int z = dir; z < (dir + 4); z++)
    {
        int dir2 = z % 4;

        if (sprite.dir != 4)
        {
            if (dir2 == ((sprite.dir + 2) % 4))
            {
                continue;
            }
        }

        int c = GetChar(sprite.x + Cx[dir2] + 48, sprite.y + Cy[dir2]);

        if (((c >= RAILBASE) && (c <= LASTRAIL)) || /* track? */
            (c == RAILVPOWERH) ||
            (c == RAILHPOWERV))
        {
            if ((sprite.dir != dir2) && (sprite.dir != 4))
            {
                if ((sprite.dir + dir2) == 3)
                {
                    sprite.frame = 3;
                }
                else
                {
                    sprite.frame = 4;
                }
            }
            else
            {
                sprite.frame = TrainPic2[dir2];
            }

            if ((c == RAILBASE) || (c == (RAILBASE + 1)))
            {
                sprite.frame = 5;
            }

            sprite.dir = dir2;
            return;
        }
    }

    if (sprite.dir == 4)
    {
        sprite.frame = 0;
        return;
    }

    sprite.dir = 4;
}


void DoCopterSprite(SimSprite* sprite)
{
    /*
    static int CDx[9] = { 0,  0,  3,  5,  3,  0, -3, -5, -3 };
    static int CDy[9] = { 0, -5, -3,  0,  3,  5,  3,  0, -3 };
    int z, d, x, y;

    if (sprite->sound_count > 0) sprite->sound_count--;

    if (sprite->control < 0) {

        if (sprite->count > 0) sprite->count--;

        if (!sprite->count) {
            // Attract copter to monster and tornado so it blows up more often
            SimSprite* s = GetSprite(GOD);
            if (s != NULL) {
                sprite->dest_x = s->x;
                sprite->dest_y = s->y;
            }
            else {
                s = GetSprite(TOR);
                if (s != NULL) {
                    sprite->dest_x = s->x;
                    sprite->dest_y = s->y;
                }
                else {
                    sprite->dest_x = sprite->orig_x;
                    sprite->dest_y = sprite->orig_y;
                }
            }
        }
        if (!sprite->count) { // land
            GetDir(sprite->x, sprite->y, sprite->orig_x, sprite->orig_y);
            if (absDist < 30) {
                sprite->frame = 0;
                return;
            }
        }
    }
    else {
        GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
        if (absDist < 16) {
            sprite->dest_x = sprite->orig_x;
            sprite->dest_y = sprite->orig_y;
            sprite->control = -1;
        }
    }

    if (!sprite->sound_count) { // send report
        x = (sprite->x + 48) >> 5;
        y = sprite->y >> 5;
        if ((x >= 0) &&
            (x < (SimWidth >> 1)) &&
            (y >= 0) &&
            (y < (SimHeight >> 1))) {
            // Don changed from 160 to 170 to shut the #$%#$% thing up!
            if ((TrfDensity[x][y] > 170) && ((Rand16() & 7) == 0)) {
                SendMesAt(NotificationId::HeavyTrafficReported, (x << 1) + 1, (y << 1) + 1);
                MakeSound("city", "HeavyTraffic"); // chopper
                sprite->sound_count = 200;
            }
        }
    }
    z = sprite->frame;
    if (!(Cycle & 3)) {
        d = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
        z = TurnTo(z, d);
        sprite->frame = z;
    }

    sprite->x += CDx[z];
    sprite->y += CDy[z];
    */
}


void DoAirplaneSprite(SimSprite* sprite)
{
    static int CDx[12] = { 0,  0,  6,  8,  6,  0, -6, -8, -6,  8,  8,  8 };
    static int CDy[12] = { 0, -8, -6,  0,  6,  8,  6,  0, -6,  0,  0,  0 };

    int z, d;

    z = sprite->frame;

    if (!(Cycle % 5))
    {
        if (z > 8)  /* TakeOff  */
        {
            z--;
            if (z < 9) z = 3;
            sprite->frame = z;
        }
        else /* goto destination */
        {
            d = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
            z = TurnTo(z, d);
            sprite->frame = z;
        }
    }

    if (absDist < 50) /* at destination  */
    {
        sprite->dest_x = RandomRange(0, SimWidth) - 50;
        sprite->dest_y = RandomRange(0, SimHeight) - 50;
    }

    /* deh added test for !Disasters */
    if (!NoDisasters)
    {
        bool explode = false;

        /*
        for (SimSprite* s = sim->sprite; s != NULL; s = s->next)
        {
            if ((s->frame != 0) && ((s->type == COP) || ((sprite != s) && (s->type == AIR))) && CheckSpriteCollision(sprite, s))
            {
                ExplodeSprite(s);
                explode = 1;
            }
        }
        */

        if (explode)
        {
            ExplodeSprite(sprite);
        }
    }

    sprite->x += CDx[z];
    sprite->y += CDy[z];

    if (SpriteNotInBounds(sprite))
    {
        sprite->frame = 0;
    }
}


void DoShipSprite(SimSprite* sprite)
{
    static int BDx[9] = { 0,  0,  1,  1,  1,  0, -1, -1, -1 };
    static int BDy[9] = { 0, -1, -1,  0,  1,  1,  1,  0, -1 };
    static int BPx[9] = { 0,  0,  2,  2,  2,  0, -2, -2, -2 };
    static int BPy[9] = { 0, -2, -2,  0,  2,  2,  2,  0, -2 };
    static int BtClrTab[8] = { RIVER, CHANNEL, POWERBASE, POWERBASE + 1,
                     RAILBASE, RAILBASE + 1, BRWH, BRWV };
    int x, y, z, t = RIVER;
    int tem, pem;

    if (sprite->sound_count > 0) sprite->sound_count--;
    if (!sprite->sound_count) {
        if ((Rand16() & 3) == 1) {
            if ((ScenarioID == 2) && /* San Francisco */
                (RandomRange(0, 10) < 5)) {
                MakeSound("city", "HonkHonk-Low -speed 80");
            }
            else {
                MakeSound("city", "HonkHonk-Low");
            }
        }
        sprite->sound_count = 200;
    }

    if (sprite->count > 0) sprite->count--;
    if (!sprite->count) {
        sprite->count = 9;
        if (sprite->frame != sprite->new_dir) {
            sprite->frame = TurnTo(sprite->frame, sprite->new_dir);
            return;
        }
        tem = Rand16() & 7;
        for (pem = tem; pem < (tem + 8); pem++) {
            z = (pem & 7) + 1;

            if (z == sprite->dir) continue;
            x = ((sprite->x + (48 - 1)) >> 4) + BDx[z];
            y = (sprite->y >> 4) + BDy[z];
            if (CoordinatesValid(x, y, SimWidth, SimHeight)) {
                t = Map[x][y] & LOMASK;
                if ((t == CHANNEL) || (t == BRWH) || (t == BRWV) ||
                    TryOther(t, sprite->dir, z)) {
                    sprite->new_dir = z;
                    sprite->frame = TurnTo(sprite->frame, sprite->new_dir);
                    sprite->dir = z + 4;
                    if (sprite->dir > 8) sprite->dir -= 8;
                    break;
                }
            }
        }
        if (pem == (tem + 8)) {
            sprite->dir = 10;
            sprite->new_dir = (Rand16() & 7) + 1;
        }
    }
    else {
        z = sprite->frame;
        if (z == sprite->new_dir) {
            sprite->x += BPx[z];
            sprite->y += BPy[z];
        }
    }
    if (SpriteNotInBounds(sprite)) {
        sprite->frame = 0;
        return;
    }
    for (z = 0; z < 8; z++) {
        if (t == BtClrTab[z]) break;
        if (z == 7) {
            ExplodeSprite(sprite);
            Destroy(sprite->x + 48, sprite->y);
        }
    }
}


void DoMonsterSprite(SimSprite* sprite)
{
    static int Gx[5] = { 2,  2, -2, -2,  0 };
    static int Gy[5] = { -2,  2,  2, -2,  0 };
    static int ND1[4] = { 0,  1,  2,  3 };
    static int ND2[4] = { 1,  2,  3,  0 };
    static int nn1[4] = { 2,  5,  8, 11 };
    static int nn2[4] = { 11,  2,  5,  8 };
    int d, z, c;

    if (sprite->sound_count > 0)
    {
        sprite->sound_count--;
    }

    if (sprite->control < 0)
    {
        /* business as usual */

        if (sprite->control == -2)
        {
            d = (sprite->frame - 1) / 3;
            z = (sprite->frame - 1) % 3;
            if (z == 2) sprite->step = 0;
            if (z == 0) sprite->step = 1;
            if (sprite->step)
            {
                z++;
            }
            else
            {
                z--;
            }

            c = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);

            if (absDist < 18)
            {
                sprite->control = -1;
                sprite->count = 1000;
                sprite->flag = 1;
                sprite->dest_x = sprite->orig_x;
                sprite->dest_y = sprite->orig_y;
            }
            else
            {
                c = (c - 1) / 2;
                if (((c != d) && (!RandomRange(0, 5))) || (!RandomRange(0, 20)))
                {
                    int diff = (c - d) & 3;

                    if ((diff == 1) || (diff == 3))
                    {
                        d = c;
                    }
                    else
                    {
                        if (Rand16() & 1) d++; else d--;
                        d &= 3;
                    }
                }
                else
                {
                    if (!RandomRange(0, 20))
                    {
                        if (Rand16() & 1)
                        {
                            d++;
                        }
                        else
                        {
                            d--;
                        }
                        d &= 3;
                    }
                }
            }
        }
        else
        {

            d = (sprite->frame - 1) / 3;

            if (d < 4) /* turn n s e w */
            {
                z = (sprite->frame - 1) % 3;

                if (z == 2)
                {
                    sprite->step = 0;
                }
                if (z == 0)
                {
                    sprite->step = 1;
                }
                if (sprite->step)
                {
                    z++;
                }
                else
                {
                    z--;
                }

                GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
                if (absDist < 60)
                {
                    if (sprite->flag == 0)
                    {
                        sprite->flag = 1;
                        sprite->dest_x = sprite->orig_x;
                        sprite->dest_y = sprite->orig_y;
                    }
                    else
                    {
                        sprite->frame = 0;
                        return;
                    }
                }

                c = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
                c = (c - 1) / 2;

                if ((c != d) && (!RandomRange(0, 10)))
                {
                    if (Rand16() & 1)
                    {
                        z = ND1[d];
                    }
                    else
                    {
                        z = ND2[d];
                    }

                    d = 4;
                    if (!sprite->sound_count)
                    {
                        MakeSound("city", "Monster -speed [MonsterSpeed]"); /* monster */
                        sprite->sound_count = 50 + RandomRange(0, 100);
                    }
                }
            }
            else
            {
                d = 4;
                c = sprite->frame;
                z = (c - 13) & 3;
                if (!(Rand16() & 3))
                {
                    if (Rand16() & 1)
                    {
                        z = nn1[z];
                    }
                    else
                    {
                        z = nn2[z];
                    }
                    d = (z - 1) / 3;
                    z = (z - 1) % 3;
                }
            }
        }
    }
    else
    {
        // somebody's taken control of the monster

        d = sprite->control;
        z = (sprite->frame - 1) % 3;

        if (z == 2)
        {
            sprite->step = 0;
        }

        if (z == 0)
        {
            sprite->step = 1;
        }

        if (sprite->step)
        {
            z++;
        }

        else
        {
            z--;
        }
    }

    z = (((d * 3) + z) + 1);
    if (z > 16)
    {
        z = 16;
    }

    sprite->frame = z;
    sprite->x += Gx[d];
    sprite->y += Gy[d];

    if (sprite->count > 0)
    {
        sprite->count--;
    }
    c = GetChar(sprite->x + sprite->x_hot, sprite->y + sprite->y_hot);
    if ((c == -1) ||
        ((c == RIVER) &&
            (sprite->count != 0) &&
            (sprite->control == -1)))
    {
        sprite->frame = 0; /* kill zilla */
    }


    /*
    for (SimSprite* s = sim->sprite; s != NULL; s = s->next)
    {
        if ((s->frame != 0) &&
            ((s->type == AIR) ||
                (s->type == COP) ||
                (s->type == SHI) ||
                (s->type == TRA)) &&
            CheckSpriteCollision(sprite, s))
        {
            ExplodeSprite(s);
        }
    }
    */

    Destroy(sprite->x + 48, sprite->y + 16);
}


void DoTornadoSprite(SimSprite& sprite)
{
    static int CDx[9] = { 2,  3,  2,  0, -2, -3 };
    static int CDy[9] = { -2,  0,  2,  3,  2,  0 };

    ++sprite.frame;
    if (sprite.frame >= sprite.frames.size())
    {
        sprite.frame = 0;
    }

    sprite.count--;

    if (sprite.count <= 0)
    {
        sprite.active = false;
    }

    /*
    for (SimSprite* s = sim->sprite; s != NULL; s = s->next)
    {
        if ((s->frame != 0) &&
            ((s->type == AIR) ||
                (s->type == COP) ||
                (s->type == SHI) ||
                (s->type == TRA)) &&
            CheckSpriteCollision(sprite, s))
        {
            ExplodeSprite(s);
        }
    }
    */

    const int newDirection = RandomRange(0, 5);
    sprite.x += CDx[newDirection];
    sprite.y += CDy[newDirection];
    if (SpriteNotInBounds(&sprite))
    {
        sprite.active = false;
    }

    if ((sprite.count != 0) && RandomRange(0, 500) == 0)
    {
        sprite.active = false;
    }

    Destroy(sprite.x + 48, sprite.y + 40);
}


void DoExplosionSprite(SimSprite *sprite)
{
  int x, y;

  if (!(Cycle & 1)) {
    if (sprite->frame == 1) {
      MakeSound("city", "Explosion-High"); /* explosion */
      x = (sprite->x >>4) + 3;
      y = (sprite->y >>4);
      SendMesAt(NotificationId::ExplosionReported, x, y);
    }
    sprite->frame++;
  }

  if (sprite->frame > 6) {
    sprite->frame = 0;

    StartFire(sprite->x + 48 - 8, sprite->y + 16);
    StartFire(sprite->x + 48 - 24, sprite->y);
    StartFire(sprite->x + 48 + 8, sprite->y);
    StartFire(sprite->x + 48 - 24, sprite->y + 32);
    StartFire(sprite->x + 48 + 8, sprite->y + 32);
    return;
  }
}


void DoBusSprite(SimSprite* sprite)
{
    /*
    static int Dx[5] = { 0,   1,   0,  -1,   0 };
    static int Dy[5] = { -1,   0,   1,   0,   0 };
    static int Dir2Frame[4] = { 1, 2, 1, 2 };
    int dx, dy, tx, ty, otx, oty;
    int turned = 0;
    int speed, z;

    if (sprite->turn)
    {
        if (sprite->turn < 0)/* ccw 
        {
            if (sprite->dir & 1) /* up or down 
            {
                sprite->frame = 4;
            }
            else /* left or right 
            {
                sprite->frame = 3;
            }
            sprite->turn++;
            sprite->dir = (sprite->dir - 1) & 3;
        }
        else /* cw 
        {
            if (sprite->dir & 1) /* up or down 
            {
                sprite->frame = 3;
            }
            else /* left or right 
            {
                sprite->frame = 4;
            }
            sprite->turn--;
            sprite->dir = (sprite->dir + 1) & 3;
        }
        turned = 1;
    }
    else
    {
        /* finish turn 
        if ((sprite->frame == 3) || (sprite->frame == 4))
        {
            turned = 1;
            sprite->frame = Dir2Frame[sprite->dir];
        }
    }

    if (sprite->speed == 0)
    {
        /* brake 
        dx = 0; dy = 0;
    }
    else /* cruise at traffic speed 
    {

        tx = (sprite->x + sprite->x_hot) >> 5;
        ty = (sprite->y + sprite->y_hot) >> 5;
        if ((tx >= 0) &&
            (tx < (SimWidth >> 1)) &&
            (ty >= 0) &&
            (ty < (SimHeight >> 1)))
        {
            z = TrfDensity[tx][ty] >> 6;
            if (z > 1) z--;
        }
        else
        {
            z = 0;
        }

        switch (z)
        {
        case 0:
            speed = 8;
            break;

        case 1:
            speed = 4;
            break;

        case 2:
            speed = 1;
            break;
        }

        /* govern speed 
        if (speed > sprite->speed)
        {
            speed = sprite->speed;
        }

        if (turned)
        {
            if (speed > 1)
            {
                speed = 1;
            }
            dx = Dx[sprite->dir] * speed;
            dy = Dy[sprite->dir] * speed;
        }
        else
        {
            dx = Dx[sprite->dir] * speed;
            dy = Dy[sprite->dir] * speed;

            tx = (sprite->x + sprite->x_hot) >> 4;
            ty = (sprite->y + sprite->y_hot) >> 4;

            /* drift into the right lane 
            switch (sprite->dir)
            {
            case 0: /* up 
                z = ((tx << 4) + 4) - (sprite->x + sprite->x_hot);
                if (z < 0) dx = -1;
                else if (z > 0) dx = 1;
                break;

            case 1: /* right 
                z = ((ty << 4) + 4) - (sprite->y + sprite->y_hot);
                if (z < 0) dy = -1;
                else if (z > 0) dy = 1;
                break;

            case 2: /* down 
                z = ((tx << 4)) - (sprite->x + sprite->x_hot);
                if (z < 0) dx = -1;
                else if (z > 0) dx = 1;
                break;

            case 3: /* left 
                z = ((ty << 4)) - (sprite->y + sprite->y_hot);
                if (z < 0) dy = -1;
                else if (z > 0) dy = 1;
                break;
            }
        }
    }

    constexpr auto AHEAD = 8;

    otx = (sprite->x + sprite->x_hot + (Dx[sprite->dir] * AHEAD)) >> 4;
    oty = (sprite->y + sprite->y_hot + (Dy[sprite->dir] * AHEAD)) >> 4;

    if (otx < 0)
    {
        otx = 0;
    }
    else if (otx >= SimWidth)
    {
        otx = SimWidth - 1;
    }

    if (oty < 0)
    {
        oty = 0;
    }
    else if (oty >= SimHeight)
    {
        oty = SimHeight - 1;
    }

    tx = (sprite->x + sprite->x_hot + dx + (Dx[sprite->dir] * AHEAD)) >> 4;
    ty = (sprite->y + sprite->y_hot + dy + (Dy[sprite->dir] * AHEAD)) >> 4;

    if (tx < 0)
    {
        tx = 0;
    }
    else if (tx >= SimWidth)
    {
        tx = SimWidth - 1;
    }

    if (ty < 0)
    {
        ty = 0;
    }
    else if (ty >= SimHeight)
    {
        ty = SimHeight - 1;
    }

    if ((tx != otx) || (ty != oty))
    {
        z = CanDriveOn(tx, ty);
        if (z == 0)
        {
            /* can't drive forward into a new tile 
            if (speed == 8)
            {
                bulldozer_tool(tx, ty);
            }
            else
            {
            }
        }
        else
        {
            /* drive forward into a new tile 
            if (z > 0)
            {
                /* smooth 
            }
            else
            {
                /* bumpy 
                dx /= 2;
                dy /= 2;
            }
        }
    }

    tx = (sprite->x + sprite->x_hot + dx) >> 4;
    ty = (sprite->y + sprite->y_hot + dy) >> 4;

    z = CanDriveOn(tx, ty);
    if (z > 0)
    {
        /* cool, cruise along 
    }
    else
    {
        if (z < 0)
        {
            /* bumpy 
        }
        else
        {
            /* something in the way 
        }
    }

    sprite->x += dx;
    sprite->y += dy;

    if (!NoDisasters)
    {
        bool explode = false;

        /*
        for (SimSprite* s = sim->sprite; s != NULL; s = s->next)
        {
            if ((sprite != s) &&
                (s->frame != 0) &&
                ((s->type == BUS) ||
                    ((s->type == TRA) &&
                        (s->frame != 5))) &&
                CheckSpriteCollision(sprite, s))
            {
                ExplodeSprite(s);
                explode = true;
            }
        }
        
        if (explode)
        {
            ExplodeSprite(sprite);
        }
    }
    */
}


void MoveObjects()
{
    if (Paused())
    {
        return;
    }

    Cycle++;

    for (size_t i = 0; i < Sprites.size(); ++i)
    {
        auto& sprite = Sprites[i];
        if (sprite.active)
        {
            switch (sprite.type)
            {
            case TRA:
                DoTrainSprite(sprite);
                break;

            case COP:
                DoCopterSprite(&sprite);
                break;

            case AIR:
                DoAirplaneSprite(&sprite);
                break;

            case SHI:
                DoShipSprite(&sprite);
                break;

            case GOD:
                DoMonsterSprite(&sprite);
                break;

            case TOR:
                DoTornadoSprite(sprite);
                break;

            case EXP:
                DoExplosionSprite(&sprite);
                break;

            case BUS:
                DoBusSprite(&sprite);
                break;
            }
        }
    }
}


void GenerateTrain(int x, int y)
{
    if (TotalPop > 20 && GetSprite(TRA) == nullptr && RandomRange(0, 25) == 0)
    {
        MakeSprite(TRA, x * 16, y * 16);
    }
}


void GenerateBus(int x, int y)
{
    /*
    if ((GetSprite(BUS) == NULL) && (!RandomRange(0, 25)))
    {
        MakeSprite(BUS, (x << 4) + BUS_GROOVE_X, (y << 4) + BUS_GROOVE_Y);
    }
    */
}


void MakeShipHere(int x, int y, int z)
{
    MakeSprite(SHI, (x << 4) - (48 - 1), (y << 4));
}


void GenerateShip()
{
    if (!(Rand16() & 3))
    {
        for (int x = 4; x < SimWidth - 2; x++)
        {
            if (Map[x][0] == CHANNEL)
            {
                MakeShipHere(x, 0, 0);
                return;
            }
        }
    }

    if (!(Rand16() & 3))
    {
        for (int y = 1; y < SimHeight - 2; y++)
        {
            if (Map[0][y] == CHANNEL)
            {
                MakeShipHere(0, y, 0);
                return;
            }
        }
    }

    if (!(Rand16() & 3))
    {
        for (int x = 4; x < SimWidth - 2; x++)
        {
            if (Map[x][SimHeight - 1] == CHANNEL)
            {
                MakeShipHere(x, SimHeight - 1, 0);
                return;
            }
        }
    }

    if (!(Rand16() & 3))
    {
        for (int y = 1; y < SimHeight - 2; y++)
        {
            if (Map[SimWidth - 1][y] == CHANNEL)
            {
                MakeShipHere(SimWidth - 1, y, 0);
                return;
            }
        }
    }
}


void MonsterHere(int x, int y)
{
  MakeSprite(GOD, (x <<4) + 48, (y <<4));
  ClearMes();
  SendMesAt(NotificationId::MonsterReported, x + 5, y);
}


void MakeMonster()
{
    /*
    bool done = false;

    SimSprite* sprite;

    if ((sprite = GetSprite(GOD)) != NULL)
    {
        sprite->sound_count = 1;
        sprite->count = 1000;
        sprite->dest_x = PolMaxX << 4;
        sprite->dest_y = PolMaxY << 4;
        return;
    }

    for (int z = 0; z < 300; z++)
    {
        const int x = RandomRange(0, SimWidth - 20) + 10;
        const int y = RandomRange(0, SimHeight - 10) + 5;
        if ((Map[x][y] == RIVER) || (Map[x][y] == RIVER + BULLBIT))
        {
            MonsterHere(x, y);
            done = true;
            break;
        }
    }

    if (!done)
    {
        MonsterHere(60, 50);
    }
    */
}

void GenerateCopter(int x, int y)
{
    /*
    if (GetSprite(COP) != nullptr)
    {
        return;
    }

    MakeSprite(COP, (x << 4), (y << 4) + 30);
    */
}


void GeneratePlane(int x, int y)
{
    /*
    if (GetSprite(AIR) != nullptr)
    {
        return;
    }

    MakeSprite(AIR, (x << 4) + 48, (y << 4) + 12);
    */
}


void MakeTornado()
{
    SimSprite* sprite{ GetSprite(TOR) };

    if (sprite != nullptr)
    {
        sprite->count = 200;
        sprite->active = true;
        //return;
    }

    int x = RandomRange(1, SimWidth - 2);
    int y = RandomRange(1, SimHeight - 2);

    MakeSprite(TOR, x * 16, y * 16);
    ClearMes();
    SendMesAt(NotificationId::TornadoReported, x, y);
    
}


void MakeExplosionAt(int x, int y)
{
    //MakeNewSprite(EXP, x - 40, y - 16);
}


void MakeExplosion(int x, int y)
{
    if ((x >= 0) && (x < SimWidth) && (y >= 0) && (y < SimHeight))
    {
        MakeExplosionAt((x << 4) + 8, (y << 4) + 8);
    }
}
