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

#include "Health.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace stride {

class Population;

/**
 * Seeds the population with Health data.
 */
class HealthSeeder
{
public:
        /// Constructor requires diease data and random number manager.
        HealthSeeder(const boost::property_tree::ptree& disease_pt, util::RNManager& rn_manager);

        /// Generate a health item by sample from distributions.
        Health Sample();

        /// Seeds the population with Health data.
        void Seed(std::shared_ptr<Population> pop);

private:
        /// Sample for each of the health data item individually.
        unsigned short int Sample(const std::vector<double>& distribution);

        /// Utility method to etract distribution from data in ptree.
        void GetDistribution(std::vector<double>& distribution, const boost::property_tree::ptree& root_pt,
                             const std::string& xml_tag);

private:
        std::vector<double>     m_distrib_start_infectiousness;
        std::vector<double>     m_distrib_start_symptomatic;
        std::vector<double>     m_distrib_time_infectious;
        std::vector<double>     m_distrib_time_symptomatic;
        std::function<double()> m_uniform01_generator;
};

} // namespace stride
