/* s_msg.c
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
#include "main.h"

#include "s_sim.h"

#include "w_resrc.h"
#include "w_sound.h"
#include "w_stubs.h"
#include "w_tk.h"


#include <algorithm>
#include <string>


int LastCityPop;
int LastCategory;
int LastPicNum;
bool AutoGo = false;
bool HaveLastMessage = false;
std::string LastMessage;


void ClearMes()
{
    MessagePort = 0;
    MesX = 0;
    MesY = 0;
    LastPicNum = 0;
}


int SendMes(int Mnum)
{
    if (Mnum < 0)
    {
        if (Mnum != LastPicNum)
        {
            MessagePort = Mnum;
            MesX = 0;
            MesY = 0;
            LastPicNum = Mnum;
            return 1;
        }
    }
    else
    {
        if (!(MessagePort))
        {
            MessagePort = Mnum;
            MesX = 0;
            MesY = 0;
            return 1;
        }
    }

    return 0;
}


void SendMesAt(int Mnum, int x, int y)
{
    if (SendMes(Mnum))
    {
        MesX = x;
        MesY = y;
    }
}


void SetMessageField(const std::string& msg)
{
    if(LastMessage.empty() || LastMessage != msg)
    {
        LastMessage = msg;
        Eval(std::string("UISetMessage {" + msg + "}").c_str());
    }
}


void DoAutoGoto(int x, int y, const std::string& msg)
{
    SetMessageField(msg);
    Eval(std::string("UIAutoGoto " + std::to_string(x) + " " + std::to_string(y)).c_str());
}


void DoShowPicture(int id)
{
    Eval(std::string("UIShowPicture " + std::to_string(id)).c_str());
}


void DoLoseGame()
{
    Eval("UILoseGame");
}


void DoWinGame()
{
    Eval("UIWinGame");
}


void DoScenarioScore(int type)
{
    int z;

    z = -200;	/* you lose */
    switch (type)
    {
    case 1:	/* Dullsville */
        if (CityClass >= 4)
        {
            z = -100;
        }
        break;

    case 2:	/* San Francisco */
        if (CityClass >= 4)
        {
            z = -100;
        }
        break;

    case 3:	/* Hamburg */
        if (CityClass >= 4)
        {
            z = -100;
        }
        break;

    case 4:	/* Bern */
        if (TrafficAverage < 80)
        {
            z = -100;
        }
        break;

    case 5:	/* Tokyo */
        if (CityScore > 500)
        {
            z = -100;
        }
        break;

    case 6:	/* Detroit */
        if (CrimeAverage < 60)
        {
            z = -100;
        }
        break;

    case 7:	/* Boston */
        if (CityScore > 500)
        {
            z = -100;
        }
        break;

    case 8:	/* Rio de Janeiro */
        if (CityScore > 500)
        {
            z = -100;
        }
        break;
    }

    ClearMes();
    SendMes(z);

    if (z == -200)
    {
        DoLoseGame();
    }
}


void CheckGrowth()
{
    int ThisCityPop;
    int z;

    if (!(CityTime % 4))
    {
        z = 0;
        ThisCityPop = ((ResPop)+(ComPop * 8L) + (IndPop * 8L)) * 20L;
        if (LastCityPop)
        {
            if ((LastCityPop < 2000) && (ThisCityPop >= 2000))	z = 35;
            if ((LastCityPop < 10000) && (ThisCityPop >= 10000)) 	z = 36;
            if ((LastCityPop < 50000L) && (ThisCityPop >= 50000L)) 	z = 37;
            if ((LastCityPop < 100000L) && (ThisCityPop >= 100000L))	z = 38;
            if ((LastCityPop < 500000L) && (ThisCityPop >= 500000L))	z = 39;
        }
        if (z)
        {
            if (z != LastCategory)
            {
                SendMes(-z);
                LastCategory = z;
            }
        }
        LastCityPop = ThisCityPop;
    }
}


void SendMessages()
{
    int z;
    int PowerPop;
    float TM;

    if ((ScenarioID) && (ScoreType) && (ScoreWait))
    {
        ScoreWait--;
        if (!ScoreWait)
        {
            DoScenarioScore(ScoreType);
        }
    }

    CheckGrowth();

    TotalZPop = ResZPop + ComZPop + IndZPop;
    PowerPop = NuclearPop + CoalPop;

    z = CityTime % 64;

    switch (z)
    {

    case 1:
        if ((TotalZPop / 4) >= ResZPop) /* need Res */
        {
            SendMes(1);
        }
        break;

    case 5:
        if ((TotalZPop >> 3) >= ComZPop) /* need Com */
        {
            SendMes(2);
        }
        break;

    case 10:
        if ((TotalZPop >> 3) >= IndZPop) /* need Ind */
        {
            SendMes(3);
        }
        break;

    case 14:
        if ((TotalZPop > 10) && ((TotalZPop << 1) > RoadTotal))
        {
            SendMes(4);
        }
        break;

    case 18:
        if ((TotalZPop > 50) && (TotalZPop > RailTotal))
        {
            SendMes(5);
        }
        break;

    case 22:
        if ((TotalZPop > 10) && (PowerPop == 0)) /* need Power */
        {
            SendMes(6);
        }
        break;

    case 26:
        if ((ResPop > 500) && (StadiumPop == 0)) /* need Stad */
        {
            SendMes(7);
            ResCap = 1;
        }
        else
        {
            ResCap = 0;
        }
        break;

    case 28:
        if ((IndPop > 70) && (PortPop == 0))
        {
            SendMes(8);
            IndCap = 1;
        }
        else IndCap = 0;
        break;

    case 30:
        if ((ComPop > 100) && (APortPop == 0))
        {
            SendMes(9);
            ComCap = 1;
        }
        else ComCap = 0;
        break;

    case 32:
        TM = static_cast<float>(unPwrdZCnt + PwrdZCnt);	/* dec score for unpowered zones */
        if (TM)
        {
            if ((PwrdZCnt / TM) < .7)
            {
                SendMes(15);
            }
        }
        break;

    case 35:
        if (PolluteAverage > /* 80 */ 60)
        {
            SendMes(-10);
        }
        break;

    case 42:
        if (CrimeAverage > 100)
        {
            SendMes(-11);
        }
        break;

    case 45:
        if ((TotalPop > 60) && (FireStPop == 0))
        {
            SendMes(13);
        }
        break;

    case 48:
        if ((TotalPop > 60) && (PolicePop == 0))
        {
            SendMes(14);
        }
        break;

    case 51:
        if (CityTax > 12)
        {
            SendMes(16);
        }
        break;

    case 54:
        if ((RoadEffect < 20) && (RoadTotal > 30))
        {
            SendMes(17);
        }
        break;

    case 57:
        if ((FireEffect < 700) && (TotalPop > 20))
        {
            SendMes(18);
        }
        break;

    case 60:
        if ((PoliceEffect < 700) && (TotalPop > 20))
        {
            SendMes(19);
        }
        break;

    case 63:
        if (TrafficAverage > 60)
        {
            SendMes(-12);
        }
        break;
    }
}


void doMessage()
{
    bool firstTime;

    if (MessagePort)
    {
        MesNum = MessagePort;
        MessagePort = 0;
        LastMesTime = TickCount();
        firstTime = true;
    }
    else
    {
        firstTime = false;
        if (MesNum == 0)  return;
        if (MesNum < 0)
        {
            MesNum = -MesNum;
            LastMesTime = TickCount();
        }
        else if ((TickCount() - LastMesTime) > (60 * 30))
        {
            MesNum = 0;
            return;
        }
    }

    if (firstTime)
    {
        switch ((MesNum < 0) ? -MesNum : MesNum)
        {
        case  12:
            if (RandomRange(0, 5) == 1)
            {
                MakeSound("city", "HonkHonk-Med");
            }
            else if (RandomRange(0, 5) == 1)
            {
                MakeSound("city", "HonkHonk-Low");
            }
            else if (RandomRange(0, 5) == 1)
            {
                MakeSound("city", "HonkHonk-High");
            }
            break;

        case 11:
        case 20:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            MakeSound("city", "Siren");
            break;

        case  21:
            MakeSound("city", "Monster -speed [MonsterSpeed]");
            break;

        case 30:
            MakeSound("city", "Explosion-Low");
            MakeSound("city", "Siren");
            break;

        case  43:
            MakeSound("city", "Explosion-High");
            MakeSound("city", "Explosion-Low");
            MakeSound("city", "Siren");
            break;

        case  44:
            MakeSound("city", "Siren");
            break;
        }
    }

    if (MesNum >= 0)
    {
        if (MesNum == 0)
        {
            return;
        }

        if (MesNum > 60)
        {
            MesNum = 0;
            return;
        }

        if (MesX || MesY)
        {
            // TODO: draw goto button
        }

        if (AutoGo && (MesX || MesY))
        {
            DoAutoGoto(MesX, MesY, GetIndString(301, MesNum));
            MesX = 0;
            MesY = 0;
        }
        else
        {
            SetMessageField(GetIndString(301, MesNum));
        }
    }
    else
    {
        // picture message
        int pictId = -(MesNum);

        DoShowPicture(pictId);

        MessagePort = pictId; // resend text message

        if (AutoGo && (MesX || MesY))
        {
            DoAutoGoto(MesX, MesY, GetIndString(301, pictId));
            MesX = 0;
            MesY = 0;
        }
    }
}
