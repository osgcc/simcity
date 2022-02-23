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
#include "w_resrc.h"

#include <map>
#include <string>


std::map<NotificationId, std::string> NotificationStringTable =
{
    { NotificationId::ResidentialNeeded, "More residential zones needed." },
    { NotificationId::CommercialNeeded, "More commercial zones needed." },
    { NotificationId::IndustrialNeeded, "More industrial zones needed." },
    { NotificationId::RoadsNeeded, "More roads required." },
    { NotificationId::RailNeeded, "Inadequate rail system." },
    { NotificationId::PowerNeeded, "Build a Power Plant." },
    { NotificationId::StadiumNeeded, "Residents demand a Stadium." },
    { NotificationId::SeaportNeeded, "Industry requires a Sea Port." },
    { NotificationId::AirportNeeded, "Commerce requires an Airport." },
    { NotificationId::PollutionHigh, "Pollution very high." },
    { NotificationId::CrimeHigh, "Crime very high." },
    { NotificationId::TrafficJamsReported, "Frequent traffic jams reported." },
    { NotificationId::FireDepartmentNeeded, "Citizens demand a Fire Department." },
    { NotificationId::PoliceDepartmentNeeded, "Citizens demand a Police Department." },
    { NotificationId::BlackoutsReported, "Blackouts reported. Check power map." },
    { NotificationId::TaxesHigh, "Citizens upset. The tax rate is too high." },
    { NotificationId::RoadsDeteriorating, "Roads deteriorating due to lack of funds." },
    { NotificationId::FireDefunded, "Fire departments need funding." },
    { NotificationId::PoliceDefunded, "Police departments need funding." },
    { NotificationId::FireReported, "Fire reported !" },
    { NotificationId::MonsterReported, "A Monster has been sighted !!" },
    { NotificationId::TornadoReported, "Tornado reported !!" },
    { NotificationId::EarthquakeReported, "Major earthquake reported !!!" },
    { NotificationId::PlaneCrashed, "A plane has crashed !" },
    { NotificationId::ShipWrecked, "Shipwreck reported !" },
    { NotificationId::TrainCrashed, "A train crashed !" },
    { NotificationId::HelicopterCrashed, "A helicopter crashed !" },
    { NotificationId::UnemploymentHigh, "Unemployment rate is high." },
    { NotificationId::CoffersEmpty, "YOUR CITY HAS GONE BROKE!" },
    { NotificationId::FirebombingReported, "Firebombing reported !" },
    { NotificationId::ParksNeeded, "Need more parks." },
    { NotificationId::ExplosionReported, "Explosion detected !" },
    { NotificationId::InsufficientFunds, "Insufficient funds to build that." },
    { NotificationId::MustBulldoze, "Area must be bulldozed first." },
    { NotificationId::ReachedTown, "Population has reached 2,000." },
    { NotificationId::ReachedCity, "Population has reached 10,000." },
    { NotificationId::ReachedCapital, "Population has reached 50,000." },
    { NotificationId::ReachedMetropolis, "Population has reached 100,000." },
    { NotificationId::ReachedMegalopolis, "Population has reached 500,000." },
    { NotificationId::BrownoutsReported, "Brownouts, build another Power Plant." },
    { NotificationId::HeavyTrafficReported, "Heavy Traffic reported." },
    { NotificationId::FloodingReported, "Flooding reported !!" },
    { NotificationId::NuclearMeltdownReported, "A Nuclear Meltdown has occurred !!!" },
    { NotificationId::RiotsReported, "They're rioting in the streets !!" },
    { NotificationId::EndOfDemo, "End of Demo !!" },
    { NotificationId::NoSoundServer, "No Sound Server!" },
    { NotificationId::NoMultiplayerLicense, "No Multi Player License !!" },
    { NotificationId::NewCityStarted, "Started a New City." },
    { NotificationId::CityRestored, "Restored a Saved City." },
    { NotificationId::None, "XXX" }
};


std::map<ZoneStatsId, std::string> ZoneStatsTable =
{
    { ZoneStatsId::Low, "Low" },
    { ZoneStatsId::Medium, "Medium" },
    { ZoneStatsId::High, "High" },
    { ZoneStatsId::VeryHigh, "Very High" },
    { ZoneStatsId::Slum, "Slum" },
    { ZoneStatsId::LowerClass, "Lower Class" },
    { ZoneStatsId::MiddleClass, "Middle Class" },
    { ZoneStatsId::HighClass, "High x" },
    { ZoneStatsId::Safe, "Safe" },
    { ZoneStatsId::Light, "Light" },
    { ZoneStatsId::Moderate, "Moderate" },
    { ZoneStatsId::Dangerous, "Dangerous" },
    { ZoneStatsId::None, "None" },
    { ZoneStatsId::Moderate2, "Moderate2" },
    { ZoneStatsId::Heavy, "Heavy" },
    { ZoneStatsId::VeryHeavy, "Very Heavy" },
    { ZoneStatsId::Declining, "Declining" },
    { ZoneStatsId::Stable, "Stable" },
    { ZoneStatsId::SlowGrowth, "Slow Growth" },
    { ZoneStatsId::FastGrowth, "Fast Growth" },
};


std::map<QueryStatsId, std::string> QueryStatsTable =
{
    { QueryStatsId::Clear, "Clear" },
    { QueryStatsId::Water, "Water" },
    { QueryStatsId::Trees, "Trees" },
    { QueryStatsId::Rubble, "Rubble" },
    { QueryStatsId::Flood, "Flood" },
    { QueryStatsId::RadioactiveWaste, "Radioactive Waste" },
    { QueryStatsId::Fire, "Fire" },
    { QueryStatsId::Road, "Road" },
    { QueryStatsId::Power, "Power" },
    { QueryStatsId::Rail, "Rail" },
    { QueryStatsId::Residential, "Residential" },
    { QueryStatsId::Commercial, "Commercial" },
    { QueryStatsId::Industrial, "Industrial" },
    { QueryStatsId::Seaport, "Seaport" },
    { QueryStatsId::Airport, "Airport" },
    { QueryStatsId::CoalPower, "Coal Power" },
    { QueryStatsId::FireDepartment, "Fire Department" },
    { QueryStatsId::PoliceDepartment, "Police Department" },
    { QueryStatsId::Stadium, "Stadium" },
    { QueryStatsId::NuclearPower, "Nuclear Power" },
    { QueryStatsId::DrawBridge, "Draw Bridge" },
    { QueryStatsId::RadarDish, "Radar Dish" },
    { QueryStatsId::Fountain, "Fountain" },
    { QueryStatsId::Industrial2, "Industrial2" },
    { QueryStatsId::Steelers38Bears3, "Steelers 38  Bears 3" },
    { QueryStatsId::DrawBridge2, "Draw Bridge2" },
    { QueryStatsId::Ur238, "Ur 238" },
    { QueryStatsId::Padding1, "" },
    { QueryStatsId::Padding2, "" }
};


const std::string& NotificationString(NotificationId id)
{
    return NotificationStringTable.at(id);
}


const std::string& ZoneStatsString(ZoneStatsId id)
{
    return ZoneStatsTable.at(id);
}


const std::string& QueryStatsString(QueryStatsId id)
{
    return QueryStatsTable.at(id);
}
