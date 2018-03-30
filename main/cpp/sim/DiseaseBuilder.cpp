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
 * Implementation for the DiseaseBuilder class.
 */

#include "DiseaseBuilder.h"

#include "core/ContactPoolSys.h"
#include "immunity/Vaccinator.h"
#include "pop/Population.h"

#include <trng/uniform_int_dist.hpp>
#include <cassert>

namespace stride {

using namespace boost::property_tree;
using namespace stride::ContactPoolType;
using namespace std;

DiseaseBuilder::DiseaseBuilder(const ptree& config_pt) : m_config_pt(config_pt) {}

void DiseaseBuilder::Build(std::shared_ptr<Simulator> sim)
{
        // --------------------------------------------------------------
        // Population immunity (natural immunity & vaccination).
        // --------------------------------------------------------------
        Vaccinator v(m_config_pt, sim->GetRNManager());
        const auto immunity_profile = m_config_pt.get<std::string>("run.immunity_profile");
        v.Administer("immunity", immunity_profile, sim->GetContactPoolSys()[ToSizeT(Id::Household)]);
        const auto vaccination_profile = m_config_pt.get<std::string>("run.vaccine_profile");
        v.Administer("vaccine", vaccination_profile, sim->GetContactPoolSys()[ToSizeT(Id::Household)]);

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

} // namespace stride
