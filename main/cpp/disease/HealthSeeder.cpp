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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation for HelthSeeder class.
 */

#include "HealthSeeder.h"

#include "pop/Population.h"
#include "util/RNManager.h"

#include <trng/uniform01_dist.hpp>

using namespace boost::property_tree;
using namespace stride::util;
using namespace std;

namespace stride {

HealthSeeder::HealthSeeder(const boost::property_tree::ptree& disease_pt, util::RNManager& rn_manager)
{
        GetDistribution(m_distrib_start_infectiousness, disease_pt, "disease.start_infectiousness");
        GetDistribution(m_distrib_start_symptomatic, disease_pt, "disease.start_symptomatic");
        GetDistribution(m_distrib_time_infectious, disease_pt, "disease.time_infectious");
        GetDistribution(m_distrib_time_symptomatic, disease_pt, "disease.time_symptomatic");
        m_uniform01_generator = rn_manager.GetGenerator(trng::uniform01_dist<double>());

        assert((abs(m_distrib_start_infectiousness.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in start_infectiousness distribution!");
        assert((abs(m_distrib_start_symptomatic.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in start_symptomatic distribution!");
        assert((abs(m_distrib_time_infectious.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in time_infectious distribution!");
        assert((abs(m_distrib_time_symptomatic.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in time_symptomatic distribution!");
}

void HealthSeeder::GetDistribution(vector<double>& distribution, const ptree& root_pt, const string& xml_tag)
{
        const boost::property_tree::ptree& subtree = root_pt.get_child(xml_tag);
        for (const auto& tree : subtree) {
                distribution.push_back(tree.second.get<double>(""));
        }
}

Health HealthSeeder::Sample()
{
        const auto start_infectiousness = Sample(m_distrib_start_infectiousness);
        const auto start_symptomatic    = Sample(m_distrib_start_symptomatic);
        const auto time_infectious      = Sample(m_distrib_time_infectious);
        const auto time_symptomatic     = Sample(m_distrib_time_symptomatic);

        return Health(start_infectiousness, start_symptomatic, time_infectious, time_symptomatic);
}

unsigned short int HealthSeeder::Sample(const vector<double>& distribution)
{
        const auto rn  = m_uniform01_generator();
        auto       ret = static_cast<unsigned short int>(distribution.size());
        for (unsigned short int i = 0; i < distribution.size(); i++) {
                if (rn <= distribution[i]) {
                        ret = i;
                        break;
                }
        }

        return ret;
}

void HealthSeeder::Seed(std::shared_ptr<stride::Population> pop)
{
        for (auto& p : *pop) {
                p.GetHealth() = Sample();
        }
}

} // namespace stride
