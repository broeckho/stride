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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Initialize populations: implementation.
 */

#include "PopBuilder.h"

#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"
#include "util/RNManager.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>

namespace stride {

using namespace std;
using namespace util;
using namespace boost::property_tree;

PopBuilder::PopBuilder(const ptree& configPt, RNManager& rnManager) : m_config_pt(configPt), m_rn_manager(rnManager) {}

shared_ptr<Population> PopBuilder::MakePoolSys(std::shared_ptr<Population> pop)
{
        using namespace ContactPoolType;
        auto& population = *pop;
        auto& poolSys    = population.GetContactPoolSys();

        // --------------------------------------------------------------
        // Determine maximum pool ids in population.
        // --------------------------------------------------------------
        IdSubscriptArray<unsigned int> max_ids{0U};
        for (const auto& p : population) {
                for (Id typ : IdList) {
                        max_ids[typ] = max(max_ids[typ], p.GetPoolId(typ));
                }
        }
        // --------------------------------------------------------------
        // Initialize poolSys with empty ContactPools (even for Id=0).
        // --------------------------------------------------------------
        for (Id typ : IdList) {
                for (size_t i = 0; i < max_ids[typ] + 1; i++) {
                        poolSys[typ].emplace_back(ContactPool(i, typ));
                }
        }

        // --------------------------------------------------------------
        // Insert persons (pointers) in their contactpools. Having Id 0
        // means "not belonging pool of that type" (e.g. school/ work -
        // cannot belong to both, or e.g. out-of-work).
        //
        // Pools are uniquely identified by (typ, subscript) and a Person
        // belongs, for typ, to pool with subscrip p.GetPoolId(typ).
        // Defensive measure: we have a pool for Id 0 and leave it empty.
        // --------------------------------------------------------------
        for (auto& p : population) {
                for (Id typ : IdList) {
                        const auto poolId = p.GetPoolId(typ);
                        if (poolId > 0) {
                                poolSys[typ][poolId].AddMember(&p);
                        }
                }
        }

        return pop;
}

shared_ptr<Population> PopBuilder::MakePersons(std::shared_ptr<Population> pop)
{
        //------------------------------------------------
        // Read persosns from file.
        //------------------------------------------------
        const auto file_name        = m_config_pt.get<string>("run.population_file");
        const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");
        const auto file_path        = (use_install_dirs) ? FileSys::GetDataDir() /= file_name : file_name;
        if (!is_regular_file(file_path)) {
                throw runtime_error(string(__func__) + "> Population file " + file_path.string() + " not present.");
        }

        boost::filesystem::ifstream pop_file;
        pop_file.open(file_path.string());
        if (!pop_file.is_open()) {
                throw runtime_error(string(__func__) + "> Error opening population file " + file_path.string());
        }

        string line;
        getline(pop_file, line); // step over file header
        unsigned int person_id = 0U;

        while (getline(pop_file, line)) {
                const auto values                 = Split(line, ",");
                const auto age                    = FromString<unsigned int>(values[0]);
                const auto household_id           = FromString<unsigned int>(values[1]);
                const auto school_id              = FromString<unsigned int>(values[2]);
                const auto work_id                = FromString<unsigned int>(values[3]);
                const auto primary_community_id   = FromString<unsigned int>(values[4]);
                const auto secondary_community_id = FromString<unsigned int>(values[5]);

                pop->CreatePerson(person_id, age, household_id, school_id, work_id, primary_community_id,
                                  secondary_community_id);
                ++person_id;
        }

        pop_file.close();
        return pop;
}

shared_ptr<Population> PopBuilder::Build(std::shared_ptr<Population> pop)
{
        //------------------------------------------------
        // Check validity of input data.
        //------------------------------------------------
        const auto seeding_rate = m_config_pt.get<double>("run.seeding_rate");
        if (seeding_rate > 1.0) {
                throw runtime_error(string(__func__) + "> Bad input data for seeding_rate.");
        }

        //------------------------------------------------
        // Add persons & fill pools & surveyseeding.
        //------------------------------------------------
        SurveySeeder(m_config_pt, m_rn_manager).Seed(MakePoolSys(MakePersons(pop)));
        return pop;
}

} // namespace stride
