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

#pragma once

#include "HouseholdReader.h"
#include "pop/Population.h"

namespace gengeopop {

/**
 * Creates a Reader that retrieves the different Household profiles from a given CSV file.
 */
class HouseholdCSVReader : public HouseholdReader
{
public:
        /// Construct the HouseholdCSVReader with an istream containing the CSV data.
        explicit HouseholdCSVReader(std::unique_ptr<std::istream> inputStream);

private:
        ///< Persons used in this Household, segmented vector to be able to have working pointers to it.
        stride::util::SegmentedVector<stride::Person> m_persons;

        ///< Contactpools used in this Household, segmented vector to be able to have working pointers to it.
        stride::util::SegmentedVector<stride::ContactPool> m_contactPools;
};

} // namespace gengeopop
