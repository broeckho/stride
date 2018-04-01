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
#include <functional>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace stride {

class Population;

/**
 * Seeds the population with survey participants.
 */
class SurveySeeder
{
public:
        /// Seeds the population with survey participants.
        ///
        /// \param config_pt         Configuration parameters.
        /// \param pop               Population.
        /// \param rn_manager        Random number manager.
        /// \param contact_logger    To log contacts/transmission events.
        static void Seed(const boost::property_tree::ptree& config_pt, std::shared_ptr<Population> pop,
                         util::RNManager& rn_manager, std::shared_ptr<spdlog::logger> contact_logger);
};

} // namespace stride
