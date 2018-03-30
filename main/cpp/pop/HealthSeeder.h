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
 * Header file for the HealthSampler
 */

#include "core/Health.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace stride {

class Population;

/**
 * Container for persons in population.
 */
class HealthSeeder
{
public:
        ///
        HealthSeeder(const boost::property_tree::ptree& disease_pt, util::RNManager& rn_manager);

        ///
        Health Sample();

        ///
        void Seed(std::shared_ptr<Population> pop);

private:
        /// Sample for each of the health data item individually.
        unsigned int Sample(const std::vector<double>& distribution);

private:
        std::vector<double>     m_distrib_start_infectiousness;
        std::vector<double>     m_distrib_start_symptomatic;
        std::vector<double>     m_distrib_time_infectious;
        std::vector<double>     m_distrib_time_symptomatic;
        std::function<double()> m_uniform01_generator;
};

} // namespace stride
