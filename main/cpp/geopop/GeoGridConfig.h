#pragma once
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

#include "contact/ContactPool.h"
#include "pop/Person.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree_fwd.hpp>

#include <memory>
#include <ostream>

namespace geopop {

class GeoGrid;
class Household;

/**
 * Configuration data mostly for generating a population, but also for computing
 * the required number od schools, workplaces, communities etc for that population.
 */
class GeoGridConfig
{
public:
        /// Default constructor needed.
        GeoGridConfig();

        /// Constructor that configures input data.
        explicit GeoGridConfig(const boost::property_tree::ptree& configPt);

        /// Prints the GeoGridconfig
        friend std::ostream& operator<<(std::ostream& stream, const GeoGridConfig& config);

        // -----------------------------------------------------------------------------------------
        // Input parameters set by constructor with configuration property tree.
        // -----------------------------------------------------------------------------------------
        struct
        {
                /// Participation of college (fraction of people of college age going to college).
                double participation_college;

                /// Participation of workplace (fraction of people of work age and not going to
                /// college having emplayment).
                double particpation_workplace;

                /// Fraction of college students that commute.
                double fraction_college_commuters;

                /// Fraction of people in the workplace that commute.
                double fraction_workplace_commuters;

                /// Target population size for the generated population.
                unsigned int pop_size;
        } input;

        // -----------------------------------------------------------------------------------------
        // The reference set of Households used to generate the population by random draws.
        // -----------------------------------------------------------------------------------------
        struct
        {
                /// Reference households: the set of households that we sample from to generate the population.
                std::vector<std::shared_ptr<Household>> households{};

                /// Persons in the reference households (segmented vector to have working pointers into it).
                stride::util::SegmentedVector<stride::Person> persons{};

                /// Contactpools used for reference households (segmented vector to have working pointers into it).
                stride::util::SegmentedVector<stride::ContactPool> pools{};
        } refHH;

        // -----------------------------------------------------------------------------------------
        // These are numbers derived from the reference households, the target size of the generated
        // population and the input parameters relating participation school and worplace.
        // These numbers are used as targets in the poggen process and are reproduced (to very close
        // approximation) in the generated population.
        // -----------------------------------------------------------------------------------------
        struct
        {
                /// Numbers of individuals in K12School.
                unsigned int popcount_k12school;

                /// Number of individuals in College.
                unsigned int popcount_college;

                /// Number of individuals in Workplace.
                unsigned int popcount_workplace;

                /// The number of households.
                unsigned int count_households;
        } popInfo;

        // -----------------------------------------------------------------------------------------
        // Config params for ContactPools (constants for now at least).
        // -----------------------------------------------------------------------------------------
        struct
        {
                /// Every houselhold constitutes a single ContactPool.
                unsigned int pools_per_houselhold = 1U;

                /// Used to calculate the number of K12Schools.
                unsigned int k12school_size      = 500U;
                unsigned int pools_per_k12school = 25U;

                /// Used to calculate the number of Colleges.
                unsigned int college_size      = 3000U;
                unsigned int pools_per_college = 20U;

                /// Used to calculate the number of Communities.
                unsigned int community_size      = 2000U;
                unsigned int pools_per_community = 1U;

                /// Used to calculate the number of Workplaces.
                unsigned int workplace_size      = 20U;
                unsigned int pools_per_workplace = 1U;
        } pools;
};

} // namespace geopop
