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
 * Header file for the HealthSeeder.
 */

#include "behaviour/belief_policies/Imitation.h"
#include "behaviour/belief_policies/NoBelief.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <functional>
#include <memory>

namespace stride {

class Population;

/**
 * Seeds the population with Health data.
 */
class BeliefSeeder
{
public:
        /// Constructor requires diease data and random number manager.
        BeliefSeeder(const boost::property_tree::ptree& configPt, util::RNManager& rnManager);

        /// Seeds the population with Health data.
        void Seed(std::shared_ptr<Population> pop);

private:
        boost::property_tree::ptree m_config_pt;
};

} // namespace stride
