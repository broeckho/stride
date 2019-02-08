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

#include "gengeopop/Household.h"
#include "util/SegmentedVector.h"

#include <memory>
#include <vector>

namespace stride {
class Person;
class ContactPool;
}

namespace gengeopop {

/**
 * Retrieves the reference Household profiles from file.
 * This can be implemented using different input file types. Currently CSV is implemented.
 */
class HouseholdReader
{
public:
        /// Construct the HouseholdReader.
        HouseholdReader()/*: m_households() */{}

        /// Default destructor.
        virtual ~HouseholdReader() = default;

        /// Add the locations to the GeoGrid.
        virtual void SetReferenceHouseholds(std::vector<std::shared_ptr<Household>>& ref_households,
                                            stride::util::SegmentedVector<stride::Person>& ref_persons,
                                            stride::util::SegmentedVector<stride::ContactPool>& ref_pools) = 0;

        /// Returns the average size of a Household.
       // double GetAverageHouseholdSize() const
        //{
        //        return static_cast<double>(m_total) / static_cast<double>(m_households.size());
        //}

        /// Returns the fraction of the population which are still of an age where they attend school.
        double GetFractionCompulsoryPupils() const
        {
                return static_cast<double>(m_totalCompulsory) / static_cast<double>(m_total);
        }

        /// Returns the fraction of the population which are between 18 and 26 years of age.
        double GetFraction1826Years() const
        {
                return static_cast<double>(m_total1826Years) / static_cast<double>(m_total);
        }

        /// Returns the fraction of the population which are between 18 and 65 years of age.
        double GetFraction1865Years() const
        {
                return static_cast<double>(m_total1865Years) / static_cast<double>(m_total);
        }


        unsigned int GetTotalPersonsInHouseholds() const { return m_total; }

        /// Return the Households found in the inputfile.
        //const std::vector<std::shared_ptr<Household>>& GetHouseHolds() const
        //{
        //        return m_households;
        //}

protected:
        //std::vector<std::shared_ptr<Household>> m_households; ///< The households which are (being) found.

        unsigned int m_total           = 0; ///< The total population.
        unsigned int m_total1826Years  = 0; ///< The total number of people between 18 and 26 years of age.
        unsigned int m_total1865Years  = 0; ///< The total number of people between 18 and 65 years of age.
        unsigned int m_totalCompulsory = 0; ///< The total number of compulsory pupils.
};

} // namespace gengeopop
