/* s_fileio.c
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

#include "s_alloc.h"
#include "s_init.h"
#include "s_sim.h"

#include "w_budget.h"
#include "w_graph.h"
#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include <iostream>
#include <string>


static bool _load_int(int& buf, size_t len, FILE* f)
{
    return fread(&buf, sizeof(int), len, f) == len;
}


static bool _save_int(int& buf, size_t len, FILE* f)
{
    return fwrite(&buf, sizeof(int), len, f) == len;
}


static int _load_file(std::string filename, char* dir)
{
    /*
  FILE *f;
  char path[512];
  int size;

  if (dir != NULL) {
    sprintf(path, "%s/%s", dir, filename);
    filename = path;
  }
  if ((f = fopen(filename.c_str(), "r")) == NULL) {
    return (0);
  }

  fseek(f, 0L, SEEK_END);
  size = ftell(f);
  fseek(f, 0L, SEEK_SET);

  switch (size) {
  case 27120: /* Normal city
    break;

  case 99120: /* 2x2 city
    break;

  case 219120: /* 3x3 city
    break;

  default:
    return (0);
  }

  if ((_load_short(ResHis, HISTLEN / 2, f) == 0) ||
      (_load_short(ComHis, HISTLEN / 2, f) == 0) ||
      (_load_short(IndHis, HISTLEN / 2, f) == 0) ||
      (_load_short(CrimeHis, HISTLEN / 2, f) == 0) ||
      (_load_short(PollutionHis, HISTLEN / 2, f) == 0) ||
      (_load_short(MoneyHis, HISTLEN / 2, f) == 0) ||
      (_load_short(MiscHis, MISCHISTLEN / 2, f) == 0) ||
      (_load_short((&Map[0][0]), WORLD_X * WORLD_Y, f) < 0)) {

    /* TODO:  report error
    fclose(f);
    return(0);
  }

  fclose(f);
      */

    return 1;
}


int loadFile(char *filename)
{
    /*
  int l;

  if (_load_file(filename, NULL) == 0)
    return(0);

  /* total funds is a int.....    MiscHis is array of shorts
  /* total funds is being put in the 50th & 51th word of MiscHis
  /* find the address, cast the ptr to a lontPtr, take contents

  l = *(int *)(MiscHis + 50);
  SetFunds(l);

  l = *(int *)(MiscHis + 8);
  CityTime = l;

  autoBulldoze = MiscHis[52];	/* flag for autoBulldoze
  autoBudget = MiscHis[53];	/* flag for autoBudget
  autoGo = MiscHis[54];		/* flag for autoGo
  UserSoundOn = MiscHis[55];	/* flag for the sound on/off
  CityTax = MiscHis[56];
  SimSpeed = MiscHis[57];
  //  sim_skips = sim_skip = 0;
  ChangeCensus();
  MustUpdateOptions = 1;

  /* yayaya

  l = *(int *)(MiscHis + 58);
  policePercent = l / 65536.0f;

  l = *(int *)(MiscHis + 60);
  firePercent = l / 65536.0f;

  l = *(int *)(MiscHis + 62);
  roadPercent = l / 65536.0f;

  policePercent = (*(int*)(MiscHis + 58)) / 65536.0f;	/* and 59
  firePercent = (*(int*)(MiscHis + 60)) / 65536.0f;	/* and 61
  roadPercent =(*(int*)(MiscHis + 62)) / 65536.0f;	/* and 63

  if (CityTime < 0)
    CityTime = 0;
  if ((CityTax > 20) || (CityTax < 0))
    CityTax = 7;
  if ((SimSpeed < 0) || (SimSpeed > 3))
    SimSpeed = 3;

  setSpeed(SimSpeed);
  setSkips(0);

  InitFundingLevel();

  /* set the scenario id to 0
  InitWillStuff();
  ScenarioID = 0;
  InitSimLoad = 1;
  DoInitialEval = 0;
  DoSimInit();
  InvalidateEditors();
  InvalidateMaps();
  */

  return 1;
}


int saveFile(const std::string& filename)
{
    /*
  int l;
  FILE *f;

  if ((f = fopen(filename.c_str(), "w")) == NULL) {
    /* TODO: report error 
    return(0);
  }

  /* total funds is a int.....    MiscHis is array of ints
  /* total funds is bien put in the 50th & 51th word of MiscHis
  /* find the address, cast the ptr to a lontPtr, take contents

  l = TotalFunds;
  (*(int *)(MiscHis + 50)) = l;

  l = CityTime;
  (*(int *)(MiscHis + 8)) = l;

  MiscHis[52] = autoBulldoze;	/* flag for autoBulldoze
  MiscHis[53] = autoBudget;	/* flag for autoBudget
  MiscHis[54] = autoGo;		/* flag for autoGo
  MiscHis[55] = UserSoundOn;	/* flag for the sound on/off
  MiscHis[57] = SimSpeed;
  MiscHis[56] = CityTax;	/* post release

  /* yayaya

  l = (int)(policePercent * 65536);
  (*(int *)(MiscHis + 58)) = l;

  l = (int)(firePercent * 65536);
  (*(int *)(MiscHis + 60)) = l;

  l = (int)(roadPercent * 65536);
  (*(int *)(MiscHis + 62)) = l;

  if ((_save_short(ResHis, HISTLEN / 2, f) == 0) ||
      (_save_short(ComHis, HISTLEN / 2, f) == 0) ||
      (_save_short(IndHis, HISTLEN / 2, f) == 0) ||
      (_save_short(CrimeHis, HISTLEN / 2, f) == 0) ||
      (_save_short(PollutionHis, HISTLEN / 2, f) == 0) ||
      (_save_short(MoneyHis, HISTLEN / 2, f) == 0) ||
      (_save_short(MiscHis, MISCHISTLEN / 2, f) == 0) ||
      (_save_short((&Map[0][0]), WORLD_X * WORLD_Y, f) < 0)) {

    /* TODO:  report error
    fclose(f);
    return(0);
  }

  fclose(f)
  */
  return 1;
}


void DidLoadScenario()
{
  Eval("UIDidLoadScenario");
}


int LoadCity(char *filename)
{
  /*
    char *cp;
  char msg[256];

  if (loadFile(filename))
  {
      CityFileName = filename;

      filename = (char *)malloc(strlen(cp) + 1);
    strcpy(filename, cp);
    setAnyCityName(filename);

    InvalidateMaps();
    InvalidateEditors();
    DidLoadCity();
    return (1);
  } else {
    sprintf(msg, "Unable to load a city from the file named \"%s\". %s",
	    filename ? filename : "(null)",
	    errno ? strerror(errno) : "");
    DidntLoadCity(msg);
    return (0);

  }
  */

    return 0;
}


void DidLoadCity()
{
  Eval("UIDidLoadCity");
}


void DidntLoadCity(const std::string& msg)
{
    std::string buf = "UIDidntLoadCity {" + msg + "}";
    Eval(buf.c_str());
}


void DoSaveCityAs()
{
    Eval("UISaveCityAs");
}


void DidSaveCity()
{
    Eval("UIDidSaveCity");
}


void DidntSaveCity(const std::string& msg)
{
    std::string buf = "UIDidntSaveCity {" + msg + "}";
    Eval(buf.c_str());
}


void SaveCityAs(const std::string& filename)
{
    /*
  char msg[256];
  char *cp;

  if (CityFileName != NULL)
    free(CityFileName);
  CityFileName = (char *)malloc(strlen(filename) + 1);
  strcpy(CityFileName, filename);

  if (saveFile(CityFileName)) {
    if (cp = (char *)rindex(filename, '.'))
      *cp = 0;
    if (cp = (char *)rindex(filename, '/'))
      cp++;
    else
      cp = filename;
    filename = (char *)malloc(strlen(cp) + 1);
    strcpy(filename, cp);
    setCityName(cp);
    DidSaveCity();
  } else {
    sprintf(msg, "Unable to save the city to the file named \"%s\". %s",
        CityFileName ? CityFileName : "(null)",
        errno ? strerror(errno) : "");
    DidntSaveCity(msg);
  }
  */
}


void SaveCity()
{
    if (CityFileName.empty())
    {
        DoSaveCityAs();
    }
    else
    {
        if (saveFile(CityFileName))
        {
            DidSaveCity();
        }
        else
        {
            DidntSaveCity("Unable to save the city to the file named '" + CityFileName + "'.");
        }
    }
}


void LoadScenario(int s)
{
    std::string name;
    std::string fname;

    SetGameLevel(0);

    if ((s < 1) || (s > 8)) { s = 1; }

    switch (s)
    {
    case 1:
        name = "Dullsville";
        fname = "snro.111";
        ScenarioID = 1;
        CityTime = ((1900 - 1900) * 48) + 2;
        SetFunds(5000);
        break;
    case 2:
        name = "San Francisco";
        fname = "snro.222";
        ScenarioID = 2;
        CityTime = ((1906 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    case 3:
        name = "Hamburg";
        fname = "snro.333";
        ScenarioID = 3;
        CityTime = ((1944 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    case 4:
        name = "Bern";
        fname = "snro.444";
        ScenarioID = 4;
        CityTime = ((1965 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    case 5:
        name = "Tokyo";
        fname = "snro.555";
        ScenarioID = 5;
        CityTime = ((1957 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    case 6:
        name = "Detroit";
        fname = "snro.666";
        ScenarioID = 6;
        CityTime = ((1972 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    case 7:
        name = "Boston";
        fname = "snro.777";
        ScenarioID = 7;
        CityTime = ((2010 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    case 8:
        name = "Rio de Janeiro";
        fname = "snro.888";
        ScenarioID = 8;
        CityTime = ((2047 - 1900) * 48) + 2;
        SetFunds(20000);
        break;
    }

    setAnyCityName(name.c_str());
    //  sim_skips = sim_skip = 0;
    InvalidateMaps();
    InvalidateEditors();
    setSpeed(3);
    CityTax = 7;

    _load_file(fname, ResourceDir);

    InitWillStuff();
    InitFundingLevel();
    UpdateFunds();
    InvalidateEditors();
    InvalidateMaps();
    InitSimLoad = 1;
    DoInitialEval = 0;
    DoSimInit();
    DidLoadScenario();
    Kick();
}
