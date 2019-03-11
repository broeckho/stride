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
#include "pop/Population.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <trng/uniform_int_dist.hpp>

namespace stride {

using namespace boost::property_tree;
using namespace stride::ContactType;
using namespace stride::util;
using namespace std;

DiseaseSeeder::DiseaseSeeder(const ptree& configPt, RnMan& rnManager) : m_config(configPt), m_rn_man(rnManager) {}

void DiseaseSeeder::Seed(std::shared_ptr<Population> pop)
{
        // --------------------------------------------------------------
        // Population immunity (natural immunity & vaccination).
        // --------------------------------------------------------------
        const auto immunityProfile = m_config.get<std::string>("run.immunity_profile");
        Vaccinate("immunity", immunityProfile, pop->GetContactPoolSys()[Id::Household]);

        const auto vaccinationProfile = m_config.get<std::string>("run.vaccine_profile");
        Vaccinate("vaccine", vaccinationProfile, pop->GetContactPoolSys()[Id::Household]);

        // --------------------------------------------------------------
        // Seed infected persons.
        // --------------------------------------------------------------
        const auto   sRate       = m_config.get<double>("run.seeding_rate");
        const auto   sAgeMin     = m_config.get<double>("run.seeding_age_min", 1);
        const auto   sAgeMax     = m_config.get<double>("run.seeding_age_max", 99);
        const auto   popSize     = pop->size();
        const auto   maxPopIndex = static_cast<unsigned int>(popSize - 1);
        auto         generator   = m_rn_man[0].variate_generator(trng::uniform_int_dist(0, maxPopIndex));
        auto&        logger      = pop->GetContactLogger();
        const string log_level   = m_config.get<string>("run.contact_log_level", "None");

        auto numInfected = static_cast<unsigned int>(floor(static_cast<double>(popSize) * sRate));
        while (numInfected > 0) {
                Person& p = pop->at(static_cast<size_t>(generator()));
                if (p.GetHealth().IsSusceptible() && (p.GetAge() >= sAgeMin) && (p.GetAge() <= sAgeMax)) {
                        p.GetHealth().StartInfection();
                        numInfected--;
                        if (log_level != "None") {
                                logger->info("[PRIM] {} {} {} {} {} {}", p.GetId(), -1, p.GetAge(), -1, -1, -1);
                        }
                }
        }
}

void DiseaseSeeder::Vaccinate(const std::string& immunityType, const std::string& immunizationProfile,
                              const SegmentedVector<ContactPool>& immunityPools)
{
        std::vector<double> immunityDistribution;
        double              linkProbability = 0;
        Immunizer           immunizer(m_rn_man);

        if (immunizationProfile == "Random") {
                const auto immunityRate = m_config.get<double>("run." + ToLower(immunityType) + "_rate");
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        immunityDistribution.push_back(immunityRate);
                }
                immunizer.Random(immunityPools, immunityDistribution, linkProbability);
        } else if (immunizationProfile == "AgeDependent") {
                const auto   immunityFile = m_config.get<string>("run." + ToLower(immunityType) + "_distribution_file");
                const ptree& immunity_pt  = FileSys::ReadPtreeFile(immunityFile);

                linkProbability = m_config.get<double>("run." + ToLower(immunityType) + "_link_probability");

                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        auto immunityRate = immunity_pt.get<double>("immunity.age" + std::to_string(index_age));
                        immunityDistribution.push_back(immunityRate);
                }
                immunizer.Random(immunityPools, immunityDistribution, linkProbability);

        } else if (immunizationProfile == "Cocoon") {
                immunizer.Cocoon(immunityPools, immunityDistribution, linkProbability);
        }
}

} // namespace stride
