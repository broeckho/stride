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
 * Implementation for the DiseaseSeeder class.
 */

#include "DiseaseSeeder.h"

#include "disease/Immunizer.h"
#include "pool/ContactPoolSys.h"
#include "pop/Population.h"
#include "sim/Simulator.h"
#include "util/StringUtils.h"

#include <trng/uniform_int_dist.hpp>
#include <cassert>

namespace stride {

using namespace boost::property_tree;
using namespace stride::ContactPoolType;
using namespace util;
using namespace std;

DiseaseSeeder::DiseaseSeeder(const ptree& config_pt, RNManager& rn_manager)
    : m_config_pt(config_pt), m_rn_manager(rn_manager)
{
}

void DiseaseSeeder::Seed(std::shared_ptr<Simulator> sim)
{
        // --------------------------------------------------------------
        // Population immunity (natural immunity & vaccination).
        // --------------------------------------------------------------
        const auto immunity_profile = m_config_pt.get<std::string>("run.immunity_profile");
        Vaccinate("immunity", immunity_profile, sim->GetContactPoolSys()[Id::Household]);

        const auto vaccination_profile = m_config_pt.get<std::string>("run.vaccine_profile");
        Vaccinate("vaccine", vaccination_profile, sim->GetContactPoolSys()[Id::Household]);

        // --------------------------------------------------------------
        // Seed infected persons.
        // --------------------------------------------------------------
        const auto seeding_rate    = m_config_pt.get<double>("run.seeding_rate");
        const auto seeding_age_min = m_config_pt.get<double>("run.seeding_age_min", 1);
        const auto seeding_age_max = m_config_pt.get<double>("run.seeding_age_max", 99);
        const auto pop_size        = sim->GetPopulation()->size() - 1;
        const auto max_pop_index   = static_cast<unsigned int>(pop_size);
        auto       int_generator   = sim->GetRNManager().GetGenerator(trng::uniform_int_dist(0, max_pop_index));

        auto num_infected = static_cast<unsigned int>(floor(static_cast<double>(pop_size + 1) * seeding_rate));
        while (num_infected > 0) {
                Person& p = sim->GetPopulation()->at(static_cast<size_t>(int_generator()));
                if (p.GetHealth().IsSusceptible() && (p.GetAge() >= seeding_age_min) &&
                    (p.GetAge() <= seeding_age_max)) {
                        p.GetHealth().StartInfection();
                        num_infected--;
                        sim->GetContactLogger()->info("[PRIM] {} {} {} {}", -1, p.GetId(), -1, 0);
                }
        }
}

void DiseaseSeeder::Vaccinate(const std::string& immunity_type, const std::string& immunization_profile,
                              std::vector<ContactPool>& immunity_pools)
{
        std::vector<double> immunity_distribution;
        const double        immunity_link_probability = 0;
        Immunizer           immunizer(m_rn_manager);

        if (immunization_profile == "Random") {
                const auto immunity_rate = m_config_pt.get<double>("run." + ToLower(immunity_type) + "_rate");
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        immunity_distribution.push_back(immunity_rate);
                }
                immunizer.Random(immunity_pools, immunity_distribution, immunity_link_probability);
        } else if (immunization_profile == "Cocoon") {
                immunizer.Cocoon(immunity_pools, immunity_distribution, immunity_link_probability);
        }
}

} // namespace stride
