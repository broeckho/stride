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

#include "BeliefSeeder.h"

#include "pop/Population.h"
#include "util/RNManager.h"

#include <boost/property_tree/ptree.hpp>
#include <trng/uniform01_dist.hpp>

using namespace boost::property_tree;
using namespace stride::util;
using namespace std;

namespace stride {

BeliefSeeder::BeliefSeeder(const boost::property_tree::ptree& configPt, util::RNManager& rnManager)
: m_config_pt(configPt)
{
}

void BeliefSeeder::Seed(std::shared_ptr<stride::Population> pop)
{
        const auto policy = m_config_pt.get<string>("run.belief_policy.name");

        // This gets extended as we add belief policies.
        if (policy == "NoBelief") {
                // some randomness in actual seeding; for later on
                for (auto& p : *pop) {
                        pop->SetBeliefPolicy<NoBelief>(NoBelief(), p);
                }
        } else if (policy == "Imitation") {
                // some randomness in actual seeding; for later on
                const Imitation imi(m_config_pt);
                for (auto& p : *pop) {
                        pop->SetBeliefPolicy<Imitation>(imi, p);
                }
        } else {
                throw runtime_error(string(__func__) + "Not a valid belief policy!");
        }
}

} // namespace stride
