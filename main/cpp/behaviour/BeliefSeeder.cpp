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
 * Implementation for BeliefSeeder class.
 */

#include "BeliefSeeder.h"

#include "pop/Population.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <trng/uniform01_dist.hpp>

using namespace boost::property_tree;
using namespace stride::util;
using namespace std;

#pragma clang diagnostic ignored "-Wunused-variable"
#pragma gcc   diagnostic ignored "-Wunused-variable"

namespace stride {

BeliefSeeder::BeliefSeeder(const boost::property_tree::ptree& configPt, util::RNManager& rnManager)
    : m_config_pt(configPt)
{
}

void BeliefSeeder::Seed(std::shared_ptr<stride::Population> pop)
{
        const auto  policy     = m_config_pt.get<string>("run.belief_policy.name");
        const auto  num_thrds  = m_config_pt.get<unsigned int>("run.num_threads");
        Population& population = *pop;

        // This gets extended as we add belief policies.
        if (policy == "NoBelief") {
                pop->InitBeliefPolicy<NoBelief>();
                const NoBelief nob;
                // some randomness in actual seeding; for later on
#pragma omp parallel for num_threads(num_thrds)
                for (size_t i = 0; i < population.size(); ++i) {
                        pop->SetBeliefPolicy<NoBelief>(i, nob);
                }
        } else if (policy == "Imitation") {
                pop->InitBeliefPolicy<Imitation>();
                // some randomness in actual seeding; for later on
                const Imitation imi(m_config_pt);
#pragma omp parallel for num_threads(num_thrds)
                for (size_t i = 0; i < population.size(); ++i) {
                        pop->SetBeliefPolicy<Imitation>(i, imi);
                }
        } else {
                throw runtime_error(string(__func__) + "Not a valid belief policy!");
        }
}

} // namespace stride
