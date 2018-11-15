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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include "Location.h"
#include "ContactCenter.h"
#include <cmath>
#include <util/Exception.h>

namespace gengeopop {
Location::Location(unsigned int id, unsigned int province, Coordinate coordinate, std::string name)
    : m_id(id), m_name(std::move(name)), m_province(province), m_population(0), m_relativePopulation(0.0),
      m_coordinate(coordinate), m_contactCenters(), m_incomingCommutingLocations(), m_outgoingCommutingLocations(),
      m_contactCenterByType()
{
}

Location::Location(unsigned int id, unsigned int province, unsigned int population, Coordinate coordinate,
                   std::string name)
    : Location(id, province, coordinate, std::move(name))
{
        m_population = population;
}

std::string Location::GetName() const { return m_name; }

unsigned int Location::GetProvince() const { return m_province; }

unsigned int Location::GetID() const { return m_id; }

unsigned int Location::GetPopulation() const { return m_population; }

double Location::GetInfectedRatio() const
{
        unsigned int infected   = 0;
        unsigned int population = 0;

        for (const std::shared_ptr<gengeopop::ContactCenter>& cc : m_contactCenters) {
                auto r = cc->GetPopulationAndInfectedCount();
                population += r.first;
                infected += r.second;
        }

        if (GetPopulation() == 0) {
                return 0;
        }

        double r = static_cast<double>(infected) / static_cast<double>(population);

        return r;
}

double Location::GetInfectedCount() const
{
        unsigned int infected = 0;

        for (const std::shared_ptr<gengeopop::ContactCenter>& cc : m_contactCenters) {
                auto r = cc->GetPopulationAndInfectedCount();
                infected += r.second;
        }

        return infected;
}

unsigned int Location::GetSimulationPopulation() const
{
        unsigned int population = 0;

        for (const std::shared_ptr<gengeopop::ContactCenter>& cc : m_contactCenters) {
                auto r = cc->GetPopulationAndInfectedCount();
                population += r.first;
        }

        return population;
}

const std::vector<std::shared_ptr<ContactCenter>>& Location::GetContactCenters() const { return m_contactCenters; }

const Coordinate& Location::GetCoordinate() const { return m_coordinate; }

void Location::SetCoordinate(const Coordinate& coordinate) { m_coordinate = coordinate; }

Location::iterator Location::begin() { return m_contactCenters.begin(); }

Location::iterator Location::end() { return m_contactCenters.end(); }

const std::vector<std::pair<Location*, double>>& Location::GetIncomingCommuningCities() const
{
        return m_incomingCommutingLocations;
}

void Location::AddIncomingCommutingLocation(std::shared_ptr<Location> location, double proportion)
{
        m_incomingCommutingLocations.emplace_back(location.get(), proportion);
}

const std::vector<std::pair<Location*, double>>& Location::GetOutgoingCommuningCities() const
{
        return m_outgoingCommutingLocations;
}

void Location::AddOutgoingCommutingLocation(std::shared_ptr<Location> location, double proportion)
{
        m_outgoingCommutingLocations.emplace_back(location.get(), proportion);
}

int Location::IncomingCommutingPeople(double fractionOfPopulationCommuting) const
{
        double value = 0;
        for (const auto& locProportion : m_incomingCommutingLocations) {
                // locProportion.second of the people in locProportion.first are commuting to this
                value += locProportion.second *
                         (fractionOfPopulationCommuting * (double)locProportion.first->GetPopulation());
        }
        return static_cast<int>(std::floor(value));
}

int Location::OutGoingCommutingPeople(double fractionOfPopulationCommuting) const
{
        double totalProportion = 0;
        for (const auto& locProportion : m_outgoingCommutingLocations) {
                // locProportion.second of the people in this are commuting to locProportion.first
                totalProportion += locProportion.second;
        }
        return static_cast<int>(
            std::floor(totalProportion * (fractionOfPopulationCommuting * static_cast<double>(m_population))));
}

bool Location::operator==(const Location& other) const
{

        using boost::geometry::get;
        return GetID() == other.GetID() && get<0>(GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(GetCoordinate()) == get<1>(other.GetCoordinate()) && GetName() == other.GetName() &&
               GetProvince() == other.GetProvince() && GetPopulation() == other.GetPopulation() &&
               GetContactCenters() == other.GetContactCenters() &&
               GetIncomingCommuningCities() == other.GetIncomingCommuningCities() &&
               GetOutgoingCommuningCities() == other.GetOutgoingCommuningCities();
}

void Location::CalculatePopulation(unsigned int totalPopulation)
{
        m_population = static_cast<unsigned int>(std::floor(m_relativePopulation * totalPopulation));
}
void   Location::SetRelativePopulation(double relativePopulation) { m_relativePopulation = relativePopulation; }
double Location::GetRelativePopulationSize() const { return m_relativePopulation; }

} // namespace gengeopop
