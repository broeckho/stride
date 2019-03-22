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
#include "contact/ContactPool.h"
#include "contact/ContactType.h"
#include "disease/Health.h"
#include "pop/Person.h"
#include "util/Exception.h"

#include <cmath>

namespace geopop {

using namespace std;
using namespace stride::ContactType;

Location::Location(unsigned int id, unsigned int province, Coordinate coordinate, string name, unsigned int popCount)
    : m_coordinate(coordinate), m_id(id), m_name(move(name)), m_pop_count(popCount), m_pop_fraction(0.0),
      m_province(province), m_inCommutes(), m_outCommutes(), m_cc(), m_pool_index()
{
}

bool Location::operator==(const Location& other) const
{
        using boost::geometry::get;

        auto temp = true;
        for (Id typ : IdList) {
                temp = temp && (CRefCenters(typ) == other.CRefCenters(typ));
                temp = temp && (CRefPools(typ) == other.CRefPools(typ));
        }
        return temp && GetID() == other.GetID() && get<0>(GetCoordinate()) == get<0>(other.GetCoordinate()) &&
               get<1>(GetCoordinate()) == get<1>(other.GetCoordinate()) && GetName() == other.GetName() &&
               GetProvince() == other.GetProvince() && GetPopCount() == other.GetPopCount() &&
               CRefIncomingCommutes() == other.CRefIncomingCommutes() &&
               CRefOutgoingCommutes() == other.CRefOutgoingCommutes();
}

void Location::AddCenter(const std::shared_ptr<ContactCenter>& contactCenter)
{
        m_cc[contactCenter->GetContactPoolType()].push_back(contactCenter);
}

void Location::AddIncomingCommute(shared_ptr<Location> otherLocation, double fraction)
{
        m_inCommutes.emplace_back(otherLocation.get(), fraction);
}

void Location::AddOutgoingCommute(shared_ptr<Location> otherLocation, double fraction)
{
        m_outCommutes.emplace_back(otherLocation.get(), fraction);
}

int Location::GetIncomingCommuteCount(double fractionCommuters) const
{
        double value = 0;
        for (const auto& locProportion : m_inCommutes) {
                // locProportion.second of the people in locProportion.first are commuting to this
                value += locProportion.second * (fractionCommuters * (double)locProportion.first->GetPopCount());
        }
        return static_cast<int>(floor(value));
}

unsigned int Location::GetInfectedCount() const
{
        unsigned int total{0U};
        for (const auto& pool : CRefPools<Id::Household>()) {
                for (const auto& person : *pool) {
                        const auto& h = person->GetHealth();
                        total += h.IsInfected() || h.IsRecovered();
                }
        }
        return total;
}

unsigned int Location::GetOutgoingCommuteCount(double fractionCommuters) const
{
        double totalProportion = 0;
        for (const auto& locProportion : m_outCommutes) {
                // locProportion.second of the people in this are commuting to locProportion.first
                totalProportion += locProportion.second;
        }
        return static_cast<unsigned int>(floor(totalProportion * (fractionCommuters * m_pop_count)));
}

double Location::GetPopFraction() const { return m_pop_fraction; }

void Location::SetPopCount(unsigned int totalPopCount)
{
        m_pop_count = static_cast<unsigned int>(floor(m_pop_fraction * totalPopCount));
}
void Location::SetPopFraction(double relativePopulation) { m_pop_fraction = relativePopulation; }

} // namespace geopop
