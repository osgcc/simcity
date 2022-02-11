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


std::map< MessageEnumerator, std::string> StringTable =
{
    { MessageEnumerator::ResidentialNeeded, "More residential zones needed." },
    { MessageEnumerator::CommercialNeeded, "More commercial zones needed." },
    { MessageEnumerator::IndustrialNeeded, "More industrial zones needed." },
    { MessageEnumerator::RoadsNeeded, "More roads required." },
    { MessageEnumerator::RailNeeded, "Inadequate rail system." },
    { MessageEnumerator::PowerNeeded, "Build a Power Plant." },
    { MessageEnumerator::StadiumNeeded, "Residents demand a Stadium." },
    { MessageEnumerator::SeaportNeeded, "Industry requires a Sea Port." },
    { MessageEnumerator::AirportNeeded, "Commerce requires an Airport." },
    { MessageEnumerator::PollutionHigh, "Pollution very high." },
    { MessageEnumerator::CrimeHigh, "Crime very high." },
    { MessageEnumerator::TrafficJamsReported, "Frequent traffic jams reported." },
    { MessageEnumerator::FireDepartmentNeeded, "Citizens demand a Fire Department." },
    { MessageEnumerator::PoliceDepartmentNeeded, "Citizens demand a Police Department." },
    { MessageEnumerator::BlackoutsReported, "Blackouts reported. Check power map." },
    { MessageEnumerator::TaxesHigh, "Citizens upset. The tax rate is too high." },
    { MessageEnumerator::RoadsDeteriorating, "Roads deteriorating due to lack of funds." },
    { MessageEnumerator::FireDefunded, "Fire departments need funding." },
    { MessageEnumerator::PoliceDefunded, "Police departments need funding." },
    { MessageEnumerator::FireReported, "Fire reported !" },
    { MessageEnumerator::MonsterReported, "A Monster has been sighted !!" },
    { MessageEnumerator::TornadoReported, "Tornado reported !!" },
    { MessageEnumerator::EarthquakeReported, "Major earthquake reported !!!" },
    { MessageEnumerator::PlaneCrashed, "A plane has crashed !" },
    { MessageEnumerator::ShipWrecked, "Shipwreck reported !" },
    { MessageEnumerator::TrainCrashed, "A train crashed !" },
    { MessageEnumerator::HelicopterCrashed, "A helicopter crashed !" },
    { MessageEnumerator::UnemploymentHigh, "Unemployment rate is high." },
    { MessageEnumerator::CoffersEmpty, "YOUR CITY HAS GONE BROKE!" },
    { MessageEnumerator::FirebombingReported, "Firebombing reported !" },
    { MessageEnumerator::ParksNeeded, "Need more parks." },
    { MessageEnumerator::ExplosionReported, "Explosion detected !" },
    { MessageEnumerator::InsufficientFunds, "Insufficient funds to build that." },
    { MessageEnumerator::MustBulldoze, "Area must be bulldozed first." },
    { MessageEnumerator::ReachedTown, "Population has reached 2,000." },
    { MessageEnumerator::ReachedCity, "Population has reached 10,000." },
    { MessageEnumerator::ReachedCapital, "Population has reached 50,000." },
    { MessageEnumerator::ReachedMetropolis, "Population has reached 100,000." },
    { MessageEnumerator::ReachedMegalopolis, "Population has reached 500,000." },
    { MessageEnumerator::BrownoutsReported, "Brownouts, build another Power Plant." },
    { MessageEnumerator::HeavyTrafficReported, "Heavy Traffic reported." },
    { MessageEnumerator::FloodingReported, "Flooding reported !!" },
    { MessageEnumerator::NuclearMeltdownReported, "A Nuclear Meltdown has occurred !!!" },
    { MessageEnumerator::RiotsReported, "They're rioting in the streets !!" },
    { MessageEnumerator::EndOfDemo, "End of Demo !!" },
    { MessageEnumerator::NoSoundServer, "No Sound Server!" },
    { MessageEnumerator::NoMultiplayerLicense, "No Multi Player License !!" },
    { MessageEnumerator::NewCityStarted, "Started a New City." },
    { MessageEnumerator::CityRestored, "Restored a Saved City." },
    { MessageEnumerator::MessageNone, "" }
};


const std::string& GetIndString(int /*id*/, MessageEnumerator id)
{
    return StringTable.at(id);
}
