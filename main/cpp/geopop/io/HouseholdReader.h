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

#include "util/SegmentedVector.h"

#include <memory>
#include <vector>

namespace stride {
class Person;
class ContactPool;
} // namespace stride

namespace geopop {

/**
 * Retrieves the reference Household profiles from file.
 * This can be implemented using different input file types. Currently CSV is implemented.
 */
class HouseholdReader
{
public:
        /// Construct the HouseholdReader.
        HouseholdReader() = default;

        /// Default destructor.
        virtual ~HouseholdReader() = default;

        /// Add the info on reference households to the GeoGridConfig.
        virtual void SetReferenceHouseholds(unsigned int&                           ref_person_count,
                                            std::vector<std::vector<unsigned int>>& ref_ages) = 0;
};

} // namespace geopop
