/* w_resrc.c:  Get resources (from files)
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
    { NotificationId::None, "" }
};


const std::string& GetIndString(int /*id*/, NotificationId id)
{
    return NotificationStringTable.at(id);
}
