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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the SurveySeeder class.
 */

#include "util/RNManager.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>

namespace stride {

class Population;

/**
 * Seeds the population with survey participants.
 */
class SurveySeeder
{
public:
        /// Initialize Seeder.
        /// \param configPt         Configuration parameters.
        /// \param rnManager         Random number manager.
        SurveySeeder(const boost::property_tree::ptree& configPt, util::RNManager& rnManager);

        /// Seeds the population with survey participants.
        /// \param pop               Population.
        std::shared_ptr<Population> Seed(std::shared_ptr<Population> pop);

private:
        const boost::property_tree::ptree& m_config_pt;  ///< Run config.
        util::RNManager&                   m_rn_manager; ///< Random number manager.
};

} // namespace stride
