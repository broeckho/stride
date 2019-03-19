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
 * Core Population class.
 */

#include "Population.h"

#include "disease/Health.h"
#include "pop/DefaultPopBuilder.h"
#include "pop/GeoPopBuilder.h"
#include "pop/ImportPopBuilder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"
#include "util/RunConfigManager.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree.hpp>
#include <utility>

using namespace boost::property_tree;
using namespace std;
using namespace stride::util;
using namespace stride::ContactType;

namespace stride {

Population::Population() : m_pool_sys(), m_contact_logger(), m_geo_grid(this) {}

std::shared_ptr<Population> Population::Create(const boost::property_tree::ptree& config, util::RnMan rnMan,
                                               std::shared_ptr<spdlog::logger> strideLogger)
{
        if (!strideLogger) {
                strideLogger = LogUtils::CreateNullLogger("Population_logger");
        }

        // --------------------------------------------------------------
        // Create empty population & and give it a ContactLogger.
        // --------------------------------------------------------------
        const auto pop = Create();
        if (config.get<bool>("run.contact_output_file", true)) {
                const auto prefix       = config.get<string>("run.output_prefix");
                const auto logPath      = FileSys::BuildPath(prefix, "contact_log.txt");
                pop->RefContactLogger() = LogUtils::CreateRotatingLogger("contact_logger", logPath.string());
                pop->RefContactLogger()->set_pattern("%v");
                strideLogger->info("Contact logging requested; logger set up.");
        } else {
                pop->RefContactLogger() = LogUtils::CreateNullLogger("contact_logger");
                strideLogger->info("No contact logging requested.");
        }

        // -----------------------------------------------------------------------------------------
        // Build population.
        // -----------------------------------------------------------------------------------------
        std::string pop_type = config.get<std::string>("run.population_type", "default");
        if (pop_type == "import") {
                strideLogger->info("Invoking ImportPopBuilder.");
                ImportPopBuilder(config, rnMan, strideLogger).Build(pop);
        } else if (pop_type == "generate") {
                strideLogger->info("Invoking GeoPopBuilder.");
                GeoPopBuilder(config, rnMan, strideLogger).Build(pop);
        } else {
                strideLogger->info("Invoking DefaultPopBuilder.");
                DefaultPopBuilder(config, rnMan, strideLogger).Build(pop);
        }

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        return pop;
}

std::shared_ptr<Population> Population::Create(const string& configString, util::RnMan rnMan,
                                               std::shared_ptr<spdlog::logger> stride_logger)
{
        return Create(RunConfigManager::FromString(configString), std::move(rnMan), std::move(stride_logger));
}

std::shared_ptr<Population> Population::Create()
{
        // --------------------------------------------------------------
        // Create (empty) population and return it
        // --------------------------------------------------------------
        struct make_shared_enabler : public Population
        {
        };
        auto r = make_shared<make_shared_enabler>();
        return r;
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

Person* Population::CreatePerson(unsigned int id, double age, unsigned int householdId, unsigned int k12SchoolId,
                                 unsigned int college, unsigned int workId, unsigned int primaryCommunityId,
                                 unsigned int secondaryCommunityId)
{
        return emplace_back(id, age, householdId, k12SchoolId, college, workId, primaryCommunityId,
                            secondaryCommunityId);
}

} // namespace stride
