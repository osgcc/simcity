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

#include "Budget.h"

#include "s_alloc.h"
#include "s_init.h"
#include "s_sim.h"

#include "w_graph.h"
#include "w_stubs.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include <iostream>
#include <string>


std::string HomeDirectory;
const std::string ResroucesDirectory;


static bool _load_int(int& buf, size_t len, FILE* f)
{
    return fread(&buf, sizeof(int), len, f) == len;
}


static bool _save_int(int& buf, size_t len, FILE* f)
{
    return fwrite(&buf, sizeof(int), len, f) == len;
}


static int _load_file(const std::string filename, const std::string& dir)
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

  if ((_load_short(ResHis, HistoryLength / 2, f) == 0) ||
      (_load_short(ComHis, HistoryLength / 2, f) == 0) ||
      (_load_short(IndHis, HistoryLength / 2, f) == 0) ||
      (_load_short(CrimeHis, HistoryLength / 2, f) == 0) ||
      (_load_short(PollutionHis, HistoryLength / 2, f) == 0) ||
      (_load_short(MoneyHis, HistoryLength / 2, f) == 0) ||
      (_load_short(MiscHis, MISCHISTLEN / 2, f) == 0) ||
      (_load_short((&Map[0][0]), SimWidth * SimHeight, f) < 0)) {

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

  if ((_save_short(ResHis, HistoryLength / 2, f) == 0) ||
      (_save_short(ComHis, HistoryLength / 2, f) == 0) ||
      (_save_short(IndHis, HistoryLength / 2, f) == 0) ||
      (_save_short(CrimeHis, HistoryLength / 2, f) == 0) ||
      (_save_short(PollutionHis, HistoryLength / 2, f) == 0) ||
      (_save_short(MoneyHis, HistoryLength / 2, f) == 0) ||
      (_save_short(MiscHis, MISCHISTLEN / 2, f) == 0) ||
      (_save_short((&Map[0][0]), SimWidth * SimHeight, f) < 0)) {

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


int LoadCity(const std::string& filename)
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
    Eval(buf);
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


enum class ScenarioName
{
    Dullsville,
    SanFransisco,
    Hamburg,
    Bern,
    Tokyo,
    Detroit,
    Boston,
    Rio
};


void LoadScenario(int s, Budget& budget)
{
    std::string name;
    std::string fname;

    GameLevel(0);

    if ((s < 1) || (s > 8)) { s = 1; }

    switch (s)
    {
    case 1:
        name = "Dullsville";
        fname = "snro.111";
        ScenarioID = 1;
        CityTime = ((1900 - 1900) * 48) + 2;
        budget.CurrentFunds(5000);
        break;

    case 2:
        name = "San Francisco";
        fname = "snro.222";
        ScenarioID = 2;
        CityTime = ((1906 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case 3:
        name = "Hamburg";
        fname = "snro.333";
        ScenarioID = 3;
        CityTime = ((1944 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case 4:
        name = "Bern";
        fname = "snro.444";
        ScenarioID = 4;
        CityTime = ((1965 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case 5:
        name = "Tokyo";
        fname = "snro.555";
        ScenarioID = 5;
        CityTime = ((1957 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case 6:
        name = "Detroit";
        fname = "snro.666";
        ScenarioID = 6;
        CityTime = ((1972 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case 7:
        name = "Boston";
        fname = "snro.777";
        ScenarioID = 7;
        CityTime = ((2010 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case 8:
        name = "Rio de Janeiro";
        fname = "snro.888";
        ScenarioID = 8;
        CityTime = ((2047 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;
    }

    setAnyCityName(name.c_str());
    //  sim_skips = sim_skip = 0;
    InvalidateMaps();
    InvalidateEditors();
    SimSpeed(SimulationSpeed::Normal);

    _load_file(fname, ResroucesDirectory);

    InitWillStuff();
    //InitFundingLevel();
    UpdateFunds(budget);
    InvalidateEditors();
    InvalidateMaps();
    InitSimLoad = 1;
    DoInitialEval = 0;
    DoSimInit(budget);
    DidLoadScenario();
}
