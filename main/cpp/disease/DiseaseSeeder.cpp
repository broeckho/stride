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

#include <boost/optional.hpp>
#include "disease/Immunizer.h"
#include "pop/Population.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>

namespace stride {

using namespace boost::property_tree;
using namespace stride::ContactType;
using namespace stride::util;
using namespace std;

DiseaseSeeder::DiseaseSeeder(const ptree& config, RnMan& rnMan) : m_config(config), m_rn_man(rnMan) {}

void DiseaseSeeder::Seed(std::shared_ptr<Population> pop)
{
        // --------------------------------------------------------------
        // Population immunity (natural immunity & vaccination).
        // --------------------------------------------------------------
        const auto immunityProfile = m_config.get<std::string>("run.immunity_profile");
        Vaccinate("immunity", immunityProfile, pop, Id::Household);

        const auto vaccinationProfile = m_config.get<std::string>("run.vaccine_profile");
        Vaccinate("vaccine", vaccinationProfile, pop, Id::Household);

        // --------------------------------------------------------------
        // Seed infected persons.
        // --------------------------------------------------------------
        const auto   sAgeMin     = m_config.get<double>("run.seeding_age_min", 1);
        const auto   sAgeMax     = m_config.get<double>("run.seeding_age_max", 99);
        const auto   popSize     = pop->size();
        const auto   maxPopIndex = static_cast<int>(popSize - 1);
        auto         generator   = m_rn_man.GetUniformIntGenerator(0, maxPopIndex, 0U);
        auto&        logger      = pop->RefContactLogger();
        const string log_level   = m_config.get<string>("run.contact_log_level", "None");

        unsigned int numInfected = 0;
        boost::optional<float> sRate = m_config.get_optional<float>("run.seeding_rate");
        if (sRate) {
        		numInfected = static_cast<unsigned int>(floor(static_cast<double>(popSize) * (*sRate)));
        } else {
        		numInfected = m_config.get<int>("run.num_index_cases");
        }


        while (numInfected > 0) {
                Person& p = pop->at(static_cast<size_t>(generator()));
                if (p.GetHealth().IsSusceptible() && (p.GetAge() >= sAgeMin) && (p.GetAge() <= sAgeMax)) {
                        p.GetHealth().StartInfection();
                        numInfected--;
                        if (log_level != "None") {
                                logger->info("[PRIM] {} {} {} {} {} {} {}", p.GetId(), -1, p.GetAge(), -1, -1, -1, p.GetPoolId(ContactType::Id::Household));
                        }
                }
        }
}

void DiseaseSeeder::Vaccinate(const std::string& immunityType, const std::string& immunizationProfile,
                              std::shared_ptr<Population> pop, const ContactType::Id contactPoolType)
{
	std::vector<double> immunityDistribution;
	double linkProbability = 0;
	Immunizer immunizer(m_rn_man);

	if (immunizationProfile == "Random") {
		const auto immunityLevel = m_config.get<double>("run." + ToLower(immunityType) + "_rate");
		for (unsigned int index_age = 0; index_age < 100; index_age++) {
			immunityDistribution.push_back(immunityLevel);
		}
		immunizer.Random(pop->CRefPoolSys().CRefPools(contactPoolType), immunityDistribution, linkProbability);
	} else if (immunizationProfile == "AgeDependent") {
		const auto immunityFile = m_config.get<string>("run." + ToLower(immunityType) + "_distribution_file");
		const ptree& immunity_pt = FileSys::ReadPtreeFile(immunityFile);

		linkProbability = m_config.get<double>("run." + ToLower(immunityType) + "_link_probability");
		for (unsigned int index_age = 0; index_age < 100; index_age++) {
			auto immunityLevel = immunity_pt.get<double>("immunity.age" + std::to_string(index_age));
			immunityDistribution.push_back(immunityLevel);
		}
		//immunizer.Random(pop->CRefPoolSys().CRefPools(contactPoolType), immunityDistribution, linkProbability);
		immunizer.Random(pop, immunityDistribution, contactPoolType, linkProbability);

	} else if (immunizationProfile == "Cocoon") {
		immunizer.Cocoon(pop->CRefPoolSys().CRefPools(contactPoolType), immunityDistribution, linkProbability);
	}
}

} // namespace stride
