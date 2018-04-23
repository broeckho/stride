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
#include "sim/Sim.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/StringUtils.h"

#include <trng/uniform_int_dist.hpp>
#include <cassert>

namespace stride {

using namespace boost::property_tree;
using namespace stride::ContactPoolType;
using namespace util;
using namespace std;

DiseaseSeeder::DiseaseSeeder(const ptree& configPt, RNManager& rnManager)
    : m_config_pt(configPt), m_rn_manager(rnManager)
{
}

void DiseaseSeeder::Seed(std::shared_ptr<Population> pop)
{
        // --------------------------------------------------------------
        // Population immunity (natural immunity & vaccination).
        // --------------------------------------------------------------
        const auto immunityProfile = m_config_pt.get<std::string>("run.immunity_profile");
        Vaccinate("immunity", immunityProfile, pop->GetContactPoolSys()[Id::Household]);

        const auto vaccinationProfile = m_config_pt.get<std::string>("run.vaccine_profile");
        Vaccinate("vaccine", vaccinationProfile, pop->GetContactPoolSys()[Id::Household]);

        // --------------------------------------------------------------
        // Seed infected persons.
        // --------------------------------------------------------------
        const auto seeding_rate    = m_config_pt.get<double>("run.seeding_rate");
        const auto seeding_age_min = m_config_pt.get<double>("run.seeding_age_min", 1);
        const auto seeding_age_max = m_config_pt.get<double>("run.seeding_age_max", 99);
        const auto pop_size        = pop->size() - 1;
        const auto max_pop_index   = static_cast<unsigned int>(pop_size);
        auto       int_generator   = m_rn_manager.GetGenerator(trng::uniform_int_dist(0, max_pop_index));
        auto&      contactLogger   = pop->GetContactLogger();

        auto num_infected = static_cast<unsigned int>(floor(static_cast<double>(pop_size + 1) * seeding_rate));
        while (num_infected > 0) {
                Person& p = pop->at(static_cast<size_t>(int_generator()));
                if (p.GetHealth().IsSusceptible() && (p.GetAge() >= seeding_age_min) &&
                    (p.GetAge() <= seeding_age_max)) {
                        p.GetHealth().StartInfection();
                        num_infected--;
                        contactLogger->info("[PRIM] {} {} {} {}", -1, p.GetId(), -1, 0);
                }
        }
}

void DiseaseSeeder::Vaccinate(const std::string& immunityType, const std::string& immunizationProfile,
                              std::vector<ContactPool>& immunityPools)
{
        std::vector<double> immunityDistribution;
        const double        linkProbability = 0;
        Immunizer           immunizer(m_rn_manager);

        if (immunizationProfile == "Random") {
                const auto immunityRate = m_config_pt.get<double>("run." + ToLower(immunityType) + "_rate");
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        immunityDistribution.push_back(immunityRate);
                }
                immunizer.Random(immunityPools, immunityDistribution, linkProbability);
        } else if (immunizationProfile == "Cocoon") {
                immunizer.Cocoon(immunityPools, immunityDistribution, linkProbability);
        }
}

} // namespace stride
