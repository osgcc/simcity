#pragma once

#include <string>

enum class NotificationId
{
    None,
    ResidentialNeeded,
    CommercialNeeded,
    IndustrialNeeded,
    RoadsNeeded,
    RailNeeded,
    PowerNeeded,
    StadiumNeeded,
    SeaportNeeded,
    AirportNeeded,
    PollutionHigh,
    CrimeHigh,
    TrafficJamsReported,
    FireDepartmentNeeded,
    PoliceDepartmentNeeded,
    BlackoutsReported,
    TaxesHigh,
    RoadsDeteriorating,
    FireDefunded,
    PoliceDefunded,
    FireReported,
    MonsterReported,
    TornadoReported,
    EarthquakeReported,
    PlaneCrashed,
    ShipWrecked,
    TrainCrashed,
    HelicopterCrashed,
    UnemploymentHigh,
    CoffersEmpty,
    FirebombingReported,
    ParksNeeded,
    ExplosionReported,
    InsufficientFunds,
    MustBulldoze,
    ReachedTown,
    ReachedCity,
    ReachedCapital,
    ReachedMetropolis,
    ReachedMegalopolis,
    BrownoutsReported,
    HeavyTrafficReported,
    FloodingReported,
    NuclearMeltdownReported,
    RiotsReported,
    EndOfDemo,
    NoSoundServer,
    NoMultiplayerLicense,
    NewCityStarted,
    CityRestored
};

enum class ZoneStatsId
{
    Low,
    Medium,
    High,
    VeryHigh,
    Slum,
    LowerClass,
    MiddleClass,
    HighClass,
    Safe,
    Light,
    Moderate,
    Dangerous,
    None,
    Moderate2,
    Heavy,
    VeryHeavy,
    Declining,
    Stable,
    SlowGrowth,
    FastGrowth
};


enum class QueryStatsId
{
    Clear,
    Water,
    Trees,
    Rubble,
    Flood,
    RadioactiveWaste,
    Fire,
    Road,
    Power,
    Rail,
    Residential,
    Commercial,
    Industrial,
    Seaport,
    Airport,
    CoalPower,
    FireDepartment,
    PoliceDepartment,
    Stadium,
    NuclearPower,
    DrawBridge,
    RadarDish,
    Fountain,
    Industrial2,
    Steelers38Bears3,
    DrawBridge2,
    Ur238
};


const std::string& NotificationString(NotificationId);
const std::string& ZoneStatsString(ZoneStatsId id);
const std::string& QueryStatsString(QueryStatsId id);
