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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the SurveySeeder class.
 */

#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>

namespace stride {

class Population;

namespace util {
class RnMan;
}

/**
 * Seeds the population with survey participants.
 */
class SurveySeeder
{
public:
        /// Initialize Seeder.
        /// \param config         Configuration parameters.
        /// \param rnMan         Random number manager.
        SurveySeeder(const boost::property_tree::ptree& config, util::RnMan& rnMan);

        /// Seeds the population with survey participants.
        /// \param pop               Population.
        std::shared_ptr<Population> Seed(std::shared_ptr<Population> pop);

private:
        const boost::property_tree::ptree& m_config; ///< Run config.
        util::RnMan&                       m_rn_man; ///< Random number manager.
};

} // namespace stride
