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
#include "s_sim.h"

#include "w_sound.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>


namespace
{
    struct ScenarioProperties
    {
        const std::string FileName{};
        const std::string CityName{};
        const int Time{};
        const int StartingFunds{};
        const int Id{};
    };

    std::map<Scenario, ScenarioProperties> ScenarioPropertiesTable
    {
        { Scenario::Dullsville, { "snro.111", "Dullsville", ((1900 - 1900) * 48) + 2, 5000, 1 } },
        { Scenario::SanFransisco, { "snro.222", "San Francisco", ((1906 - 1900) * 48) + 2, 20000, 2 } },
        { Scenario::Hamburg, { "snro.333", "Hamburg", ((1944 - 1900) * 48) + 2, 20000, 3 } },
        { Scenario::Bern, { "snro.444", "Bern", ((1965 - 1900) * 48) + 2, 20000, 4 } },
        { Scenario::Tokyo, { "snro.555", "Tokyo", ((1957 - 1900) * 48) + 2, 20000, 5 } },
        { Scenario::Detroit, { "snro.666", "Detroit", ((1972 - 1900) * 48) + 2, 20000, 6 } },
        { Scenario::Boston, { "snro.777", "Boston", ((2010 - 1900) * 48) + 2, 20000, 7 } },
        { Scenario::Rio, { "snro.888", "Rio de Janeiro", ((2047 - 1900) * 48) + 2, 20000, 8 } }
    };

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
    budget.PreviousFunds(MiscHis[51]);
    AutoBulldoze = MiscHis[52];
    autoBudget(MiscHis[53]);
    autoGoto(MiscHis[54]);

    userSoundOn(MiscHis[55]);
    budget.TaxRate(std::clamp(MiscHis[56], 0, 20));
    SimSpeed(static_cast<SimulationSpeed>(MiscHis[57]));

    budget.PolicePercent(static_cast<float>(MiscHis[58] / 100.0f));
    budget.FirePercent(static_cast<float>(MiscHis[60] / 100.0f));
    budget.RoadPercent(static_cast<float>(MiscHis[62] / 100.0f));

    initWillStuff();
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
    MiscHis[51] = budget.PreviousFunds();

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
    properties.GameLevel(0);

    const auto& scenarioProperties = ScenarioPropertiesTable.at(scenario);

    properties.CityName(scenarioProperties.CityName);
    budget.CurrentFunds(scenarioProperties.StartingFunds);
    CityTime = scenarioProperties.Time;
    ScenarioID = scenarioProperties.Id;

    _load_file("scenarios/" + scenarioProperties.FileName);

    SimSpeed(SimulationSpeed::Normal);

    initWillStuff();
    UpdateFunds(budget);
    InitSimLoad = 1;
    DoInitialEval = 0;
    DoSimInit(properties, budget);
}
