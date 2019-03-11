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

Population::Population() : m_pool_sys(), m_contact_logger(), m_geoGrid(), m_currentContactPoolId()
{
        for (Id typ : IdList) {
                m_pool_sys[typ].emplace_back(ContactPool(0U, typ));
                m_currentContactPoolId[typ] = 1;
        }
}

std::shared_ptr<Population> Population::Create(const boost::property_tree::ptree& configPt, util::RnMan& rnManager,
                                               std::shared_ptr<spdlog::logger> stride_logger)
{
        if (!stride_logger) {
                stride_logger = LogUtils::CreateNullLogger("Population_logger");
        }

        // --------------------------------------------------------------
        // Create (empty) population & and give it a ContactLogger.
        // --------------------------------------------------------------
        const auto pop = Create();
        if (configPt.get<bool>("run.contact_output_file", true)) {
                const auto prefix       = configPt.get<string>("run.output_prefix");
                const auto logPath      = FileSys::BuildPath(prefix, "contact_log.txt");
                pop->GetContactLogger() = LogUtils::CreateRotatingLogger("contact_logger", logPath.string());
                pop->GetContactLogger()->set_pattern("%v");
                stride_logger->info("Contact logging requested; logger set up.");
        } else {
                pop->GetContactLogger() = LogUtils::CreateNullLogger("contact_logger");
                stride_logger->info("No contact logging requested.");
        }

        // -----------------------------------------------------------------------------------------
        // Build population.
        // -----------------------------------------------------------------------------------------
        std::string pop_type = configPt.get<std::string>("run.population_type", "default");
        if (pop_type == "import") {
                stride_logger->info("ImportPopBuilder invoked.");
                ImportPopBuilder(configPt, rnManager).Build(pop);
        } else if (pop_type == "generate") {
                stride_logger->info("GeoPopBuilder invoked.");
                GeoPopBuilder(configPt, rnManager).Build(pop);
        } else {
                stride_logger->info("DefaultPopBuilder invoked.");
                DefaultPopBuilder(configPt, rnManager).Build(pop);
        }

        // -----------------------------------------------------------------------------------------
        // Done.
        // -----------------------------------------------------------------------------------------
        return pop;
}

std::shared_ptr<Population> Population::Create(const string& configString, util::RnMan& rnManager,
                                               std::shared_ptr<spdlog::logger> stride_logger)
{
        return Create(RunConfigManager::FromString(configString), rnManager, std::move(stride_logger));
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

ContactPool* Population::CreateContactPool(ContactType::Id typeId)
{
        return m_pool_sys[typeId].emplace_back(m_currentContactPoolId[typeId]++, typeId);
}

} // namespace stride
