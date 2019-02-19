/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "Location.h"
#include "ContactCenter.h"
#include "util/Exception.h"

#include <cmath>

namespace geopop {

using namespace std;

Location::Location(unsigned int id, unsigned int province, Coordinate coordinate, string name)
    : m_id(id), m_name(move(name)), m_province(province), m_pop_count(0), m_pop_fraction(0.0), m_coordinate(coordinate),
      m_CC(), m_inCommuteLocations(), m_outCommuteLocations(), m_CC_OfType()
{
}

Location::Location(unsigned int id, unsigned int province, unsigned int popCount, Coordinate coordinate, string name)
    : Location(id, province, coordinate, move(name))
{
        m_pop_count = popCount;
}

bool Location::operator==(const Location& other) const
{
        using boost::geometry::get;
        return GetID() == other.GetID() && get<0>(GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(GetCoordinate()) == get<1>(other.GetCoordinate()) && GetName() == other.GetName() &&
               GetProvince() == other.GetProvince() && GetPopCount() == other.GetPopCount() &&
               GetContactCenters() == other.GetContactCenters() &&
               GetIncomingCommuningCities() == other.GetIncomingCommuningCities() &&
               GetOutgoingCommutingCities() == other.GetOutgoingCommutingCities();
}

void Location::AddIncomingCommutingLocation(shared_ptr<Location> otherLocation, double fraction)
{
        m_inCommuteLocations.emplace_back(otherLocation.get(), fraction);
}

void Location::AddOutgoingCommutingLocation(shared_ptr<Location> otherLocation, double fraction)
{
        m_outCommuteLocations.emplace_back(otherLocation.get(), fraction);
}

const vector<pair<Location*, double>>& Location::GetIncomingCommuningCities() const { return m_inCommuteLocations; }

int Location::GetIncomingCommuterCount(double fractionCommuters) const
{
        double value = 0;
        for (const auto& locProportion : m_inCommuteLocations) {
                // locProportion.second of the people in locProportion.first are commuting to this
                value += locProportion.second * (fractionCommuters * (double)locProportion.first->GetPopCount());
        }
        return static_cast<int>(floor(value));
}

double Location::GetInfectedCount() const
{
        auto infected = 0U;

        for (const auto& cc : m_CC) {
                const auto r = cc->GetPopulationAndInfectedCount();
                infected += r.second;
        }

        return infected;
}

const vector<pair<Location*, double>>& Location::GetOutgoingCommutingCities() const { return m_outCommuteLocations; }

int Location::GetOutgoingCommuterCount(double fractionCommuters) const
{
        double totalProportion = 0;
        for (const auto& locProportion : m_outCommuteLocations) {
                // locProportion.second of the people in this are commuting to locProportion.first
                totalProportion += locProportion.second;
        }
        return static_cast<int>(floor(totalProportion * (fractionCommuters * static_cast<double>(m_pop_count))));
}

double Location::GetRelativePopulationSize() const { return m_pop_fraction; }

void Location::SetPopCount(unsigned int totalPopCount)
{
        m_pop_count = static_cast<unsigned int>(floor(m_pop_fraction * totalPopCount));
}
void Location::SetRelativePopulation(double relativePopulation) { m_pop_fraction = relativePopulation; }

} // namespace geopop
