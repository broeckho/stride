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
 * Core Population class
 */

#include "Population.h"

#include "behaviour/belief_policies/Imitation.h"
#include "behaviour/belief_policies/NoBelief.h"
#include "disease/Health.h"
#include "pop/PopBuilder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/RNManager.h"
#include "util/RunConfigManager.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree.hpp>
#include <utility>

using namespace boost::property_tree;
using namespace std;
using namespace stride::util;

namespace stride {

std::shared_ptr<Population> Population::Create(const boost::property_tree::ptree& configPt)
{
        // --------------------------------------------------------------
        // Create (empty) population & and give it a ContactLogger.
        // --------------------------------------------------------------
        struct make_shared_enabler : public Population
        {
        };
        auto pop = make_shared<make_shared_enabler>();
        if (configPt.get<bool>("run.contact_output_file", true)) {
                const auto prefix       = configPt.get<string>("run.output_prefix");
                const auto logPath      = FileSys::BuildPath(prefix, "contact_log.txt");
                pop->GetContactLogger() = LogUtils::CreateRotatingLogger("contact_logger", logPath.string());
                pop->GetContactLogger()->set_pattern("%v");
        } else {
                pop->GetContactLogger() = LogUtils::CreateNullLogger("contact_logger");
        }

        // ------------------------------------------------
        // Setup RNManager.
        // ------------------------------------------------
        RNManager rnManager(RNManager::Info{configPt.get<string>("pop.rng_type", "lcg64"),
                                            configPt.get<unsigned long>("run.rng_seed", 101UL), "",
                                            configPt.get<unsigned int>("run.num_threads")});

        // -----------------------------------------------------------------------------------------
        // Build population (at later date multiple builder or build instances ...).
        // -----------------------------------------------------------------------------------------
        return PopBuilder(configPt, rnManager).Build(pop);
}

std::shared_ptr<Population> Population::Create(const string& configString)
{
        return Create(RunConfigManager::FromString(configString));
}

unsigned int Population::GetAdoptedCount() const
{
        unsigned int total{0U};
        for (const auto& p : *this) {
                if (p.GetBelief()->HasAdopted()) {
                        total++;
                }
        }
        return total;
}

unsigned int Population::GetInfectedCount() const
{
        unsigned int total{0U};
        for (const auto& p : *this) {
                const auto& h = p.GetHealth();
                total += h.IsInfected() || h.IsRecovered();
        }
        return total;
}

void Population::CreatePerson(unsigned int id, double age, unsigned int householdId, unsigned int schoolId,
                              unsigned int workId, unsigned int primaryCommunityId, unsigned int secondaryCommunityId)
{
        this->emplace_back(Person(id, age, householdId, schoolId, workId, primaryCommunityId, secondaryCommunityId));
}

} // namespace stride
