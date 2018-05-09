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
 *  Copyright 2017, 2018 Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation for HelthSeeder class.
 */

#include "HealthSeeder.h"

#include "pop/Population.h"
#include "util/RNManager.h"

#include <omp.h>
#include <trng/uniform01_dist.hpp>

using namespace boost::property_tree;
using namespace stride::util;
using namespace std;

namespace stride {

HealthSeeder::HealthSeeder(const boost::property_tree::ptree& diseasePt)
{
        GetDistribution(m_distrib_start_infectiousness, diseasePt, "disease.start_infectiousness");
        GetDistribution(m_distrib_start_symptomatic, diseasePt, "disease.start_symptomatic");
        GetDistribution(m_distrib_time_infectious, diseasePt, "disease.time_infectious");
        GetDistribution(m_distrib_time_symptomatic, diseasePt, "disease.time_symptomatic");

        assert((abs(m_distrib_start_infectiousness.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in start_infectiousness distribution!");
        assert((abs(m_distrib_start_symptomatic.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in start_symptomatic distribution!");
        assert((abs(m_distrib_time_infectious.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in time_infectious distribution!");
        assert((abs(m_distrib_time_symptomatic.back() - 1.0) < 1.e-10) &&
               "HealthSampler> Error in time_symptomatic distribution!");
}

void HealthSeeder::GetDistribution(vector<double>& distribution, const ptree& rootPt, const string& xmlTag)
{
        const boost::property_tree::ptree& subtree = rootPt.get_child(xmlTag);
        for (const auto& tree : subtree) {
                distribution.push_back(tree.second.get<double>(""));
        }
}

unsigned short int HealthSeeder::Sample(const vector<double>& distribution, double random01)
{
        auto ret = static_cast<unsigned short int>(distribution.size());
        for (unsigned short int i = 0; i < distribution.size(); i++) {
                if (random01 <= distribution[i]) {
                        ret = i;
                        break;
                }
        }
        return ret;
}

void HealthSeeder::Seed(const std::shared_ptr<stride::Population>& pop, vector<ContactHandler>& handlers)
{
        auto& population = *pop;

#pragma omp parallel num_threads(handlers.size())
        {
                auto& gen01 = handlers[static_cast<size_t>(omp_get_thread_num())];
#pragma omp for
                for (size_t i = 0; i < population.size(); ++i) {
                        const auto startInfectiousness = Sample(m_distrib_start_infectiousness, gen01());
                        const auto startSymptomatic = Sample(m_distrib_start_symptomatic, gen01());
                        const auto timeInfectious = Sample(m_distrib_time_infectious, gen01());
                        const auto timeSymptomatic = Sample(m_distrib_time_symptomatic, gen01());
                        population[i].GetHealth()
                                = Health(startInfectiousness, startSymptomatic, timeInfectious, timeSymptomatic);
                }
        }
}

} // namespace stride
