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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Initialize populations: implementation.
 */

#include "DefaultPopBuilder.h"

#include "contact/ContactLogMode.h"
#include "contact/ContactType.h"
#include "contact/IdSubscriptArray.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"
#include "util/RnMan.h"
#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <fstream>

namespace stride {

using namespace ContactType;

using namespace util;
using namespace boost::property_tree;
using namespace std;

shared_ptr<Population> DefaultPopBuilder::MakePersons(shared_ptr<Population> pop)
{
        //------------------------------------------------
        // Read persons from file.
        //------------------------------------------------
        const auto fileName = m_config.get<string>("run.population_file");
        m_stride_logger->info("Building default population from file {}.", fileName);

        const auto use_install_dirs = m_config.get<bool>("run.use_install_dirs");
        const auto filePath         = (use_install_dirs) ? FileSys::GetDataDir() /= fileName : filesys::path(fileName);
        if (!is_regular_file(filePath)) {
                throw runtime_error(string(__func__) + "> Population file " + filePath.string() + " not present.");
        }

        ifstream popFile;
        popFile.open(filePath.string());
        if (!popFile.is_open()) {
                throw runtime_error(string(__func__) + "> Error opening population file " + filePath.string());
        }

        string line;
        getline(popFile, line); // step over file header
        unsigned int person_id = 0U;

        while (getline(popFile, line)) {
                const auto values               = Split(line, ",");
                const auto age                  = FromString<unsigned int>(values[0]);
                const auto householdId          = FromString<unsigned int>(values[1]);
                const auto schoolId             = FromString<unsigned int>(values[2]);
                const auto workId               = FromString<unsigned int>(values[3]);
                const auto primaryCommunityId   = FromString<unsigned int>(values[4]);
                const auto secondaryCommunityId = FromString<unsigned int>(values[5]);

                pop->CreatePerson(person_id, age, householdId, schoolId, 0, workId, primaryCommunityId,
                                  secondaryCommunityId);
                ++person_id;
        }

        popFile.close();

        m_stride_logger->trace("Done building default population.");

        return pop;
}

shared_ptr<Population> DefaultPopBuilder::Build(shared_ptr<Population> pop)
{
        //------------------------------------------------
        // Check validity of input data.
        //------------------------------------------------
        const auto seedingRate = m_config.get<double>("run.seeding_rate");
        if (seedingRate > 1.0) {
                throw runtime_error(string(__func__) + "> Bad input data for seeding_rate.");
        }

        //------------------------------------------------
        // Add persons
        //------------------------------------------------
        MakePersons(pop);

        // --------------------------------------------------------------
        // Determine maximum pool ids in population.
        // --------------------------------------------------------------
        IdSubscriptArray<unsigned int> maxIds{0U};
        for (const auto& p : *pop) {
                for (Id typ : IdList) {
                        maxIds[typ] = max(maxIds[typ], p.GetPoolId(typ));
                }
        }
        // --------------------------------------------------------------
        // Initialize poolSys with empty ContactPools (even for Id=0).
        // --------------------------------------------------------------
        for (Id typ : IdList) {
                for (unsigned int i = 1; i < maxIds[typ] + 1; i++) {
                        pop->RefPoolSys().CreateContactPool(typ);
                }
        }

        // --------------------------------------------------------------
        // Insert persons (pointers) in their contactpools. Having Id 0
        // means "not belonging pool of that type" (e.g. school/ work -
        // cannot belong to both, or e.g. out-of-work).
        //
        // Pools are uniquely identified by (typ, subscript) and a Person
        // belongs, for typ, to pool with subscript p.GetPoolId(typ).
        // Defensive measure: we have a pool for Id 0 and leave it empty.
        // --------------------------------------------------------------
        for (auto& p : *pop) {
                for (Id typ : IdList) {
                        const auto poolId = p.GetPoolId(typ);
                        if (poolId > 0) {
                                pop->RefPoolSys().RefPools(typ)[poolId].AddMember(&p);
                        }
                }
        }


        return pop;
}

} // namespace stride
