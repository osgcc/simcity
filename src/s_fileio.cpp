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
#include "CityProperties.h"
#include "Map.h"

#include "s_alloc.h"
#include "s_fileio.h"
#include "s_init.h"
#include "s_sim.h"

#include "w_sound.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>


namespace
{
    void copyBufIntoArray(const int (&buf)[HistoryLength], GraphHistory& graph)
    {
        for (size_t i = 0; i < ResHis.size(); ++i)
        {
            graph[i] = buf[i];
        }
    }

    bool _load_file(const std::string filename)
    {
        std::ifstream infile(filename, std::ofstream::binary);
        if (infile.fail())
        {
            infile.close();
            return false;
        }

        int buff[HistoryLength]{};

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, ResHis);

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, ComHis);

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, IndHis);

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, CrimeHis);

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, PollutionHis);

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, MoneyHis);

        infile.read(reinterpret_cast<char*>(&buff[0]), sizeof(GraphHistory));
        copyBufIntoArray(buff, MiscHis);

        int mapRow[SimHeight]{};
        for (size_t row = 0; row < SimWidth; ++row)
        {
            infile.read(reinterpret_cast<char*>(&mapRow[0]), sizeof(mapRow));

            for (size_t i = 0; i < SimHeight; ++i)
            {
                Map[row][i] = mapRow[i];
            }
        }
           
        infile.close();

        return true;
    }
}


bool loadFile(const std::string& filename, CityProperties& properties, Budget& budget)
{
    if (!_load_file(filename))
    {
        return false;
    }

    CityTime = std::clamp(MiscHis[8], 0, std::numeric_limits<int>::max());
    budget.CurrentFunds(MiscHis[50]);
    AutoBulldoze = MiscHis[52];
    autoBudget(MiscHis[53]);
    autoGoto(MiscHis[54]);

    userSoundOn(MiscHis[55]);
    budget.TaxRate(std::clamp(MiscHis[56], 0, 20));
    SimSpeed(static_cast<SimulationSpeed>(MiscHis[57]));

    budget.PolicePercent(static_cast<float>(MiscHis[58] / 100.0f));
    budget.FirePercent(static_cast<float>(MiscHis[60] / 100.0f));
    budget.RoadPercent(static_cast<float>(MiscHis[62] / 100.0f));

    InitWillStuff();
    ScenarioID = 0;
    DoSimInit(properties, budget);

    return true;
}


bool saveFile(const std::string& filename, const CityProperties&, const Budget& budget)
{
    std::ofstream outfile(filename, std::ofstream::binary);
    if (outfile.fail())
    {
        outfile.close();
        return false;
    }

    MiscHis[8] = CityTime;
    MiscHis[50] = budget.CurrentFunds();

    MiscHis[52] = AutoBulldoze;
    MiscHis[53] = autoBudget(); 
    MiscHis[54] = autoGoto();
    MiscHis[55] = userSoundOn();
    MiscHis[57] = static_cast<int>(SimSpeed());
    MiscHis[56] = budget.TaxRate();

    MiscHis[58] = static_cast<int>(budget.PolicePercent() * 100.0f);
    MiscHis[60] = static_cast<int>(budget.FirePercent() * 100.0f);
    MiscHis[62] = static_cast<int>(budget.RoadPercent() * 100.0f);

    outfile.write(reinterpret_cast<char*>(ResHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(ComHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(IndHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(CrimeHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(PollutionHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(MoneyHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(MiscHis.data()), sizeof(GraphHistory));
    outfile.write(reinterpret_cast<char*>(Map.data()), sizeof(Map));

    outfile.close();
    return true;
}


bool LoadCity(const std::string& filename, CityProperties& properties, Budget& budget)
{
    if(!loadFile(filename, properties, budget))
    {
        std::cout << "Unable to load a city from the file named '" << filename << "'" << std::endl;
        return false;
    }

    return true;
}


void SaveCity(const std::string& filename, const CityProperties& properties, const Budget& budget)
{
    if (saveFile(filename, properties, budget))
    {
        std::cout << "City saved as '" << filename << "'" << std::endl;
    }
    else
    {
        std::cout << "Unable to save the city to the file named '" << filename << "'" << std::endl;
    }
}


void LoadScenario(Scenario scenario, CityProperties& properties, Budget& budget)
{
    std::string fname;

    properties.GameLevel(0);

    switch (scenario)
    {
    case Scenario::Dullsville:
        properties.CityName("Dullsville");
        fname = "snro.111";
        ScenarioID = 1;
        CityTime = ((1900 - 1900) * 48) + 2;
        budget.CurrentFunds(5000);
        break;

    case Scenario::SanFransisco:
        properties.CityName("San Francisco");
        fname = "snro.222";
        ScenarioID = 2;
        CityTime = ((1906 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case Scenario::Hamburg:
        properties.CityName("Hamburg");
        fname = "snro.333";
        ScenarioID = 3;
        CityTime = ((1944 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case Scenario::Bern:
        properties.CityName("Bern");
        fname = "snro.444";
        ScenarioID = 4;
        CityTime = ((1965 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case Scenario::Tokyo:
        properties.CityName("Tokyo");
        fname = "snro.555";
        ScenarioID = 5;
        CityTime = ((1957 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case Scenario::Detroit:
        properties.CityName("Detroit");
        fname = "snro.666";
        ScenarioID = 6;
        CityTime = ((1972 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case Scenario::Boston:
        properties.CityName("Boston");
        fname = "snro.777";
        ScenarioID = 7;
        CityTime = ((2010 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;

    case Scenario::Rio:
        properties.CityName("Rio de Janeiro");
        fname = "snro.888";
        ScenarioID = 8;
        CityTime = ((2047 - 1900) * 48) + 2;
        budget.CurrentFunds(20000);
        break;
    }

    SimSpeed(SimulationSpeed::Normal);

    _load_file("scenarios/" + fname);

    InitWillStuff();
    UpdateFunds(budget);
    InitSimLoad = 1;
    DoInitialEval = 0;
    DoSimInit(properties, budget);
}
