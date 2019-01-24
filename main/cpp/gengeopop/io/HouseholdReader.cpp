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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "HouseholdReader.h"

namespace gengeopop {

HouseholdReader::HouseholdReader() : m_households() {}

const std::vector<std::shared_ptr<Household>>& gengeopop::HouseholdReader::GetHouseHolds() const
{
        return m_households;
}

unsigned int HouseholdReader::GetTotalPersonsInHouseholds() const { return m_total; }

double HouseholdReader::AverageHouseholdSize() const
{
        return static_cast<double>(m_total) / static_cast<double>(m_households.size());
}

double HouseholdReader::GetFraction1826Years() const
{
        return static_cast<double>(m_total1826Years) / static_cast<double>(m_total);
}

double HouseholdReader::GetFractionCompulsoryPupils() const
{
        return static_cast<double>(m_totalCompulsory) / static_cast<double>(m_total);
}

double HouseholdReader::GetFraction1865Years() const
{
        return static_cast<double>(m_total1865Years) / static_cast<double>(m_total);
}

} // namespace gengeopop
