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

#include "disease/Health.h"
#include "pop/Population.h"
#include "pop/SurveySeeder.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"
#include "util/StringUtils.h"

#include <trng/uniform_int_dist.hpp>

namespace stride {

using namespace std;
using namespace util;
using namespace boost::property_tree;

PopBuilder::PopBuilder(const ptree& configPt, std::shared_ptr<spdlog::logger> logger)
    : m_config_pt(configPt), m_pop(make_shared<Population>()), m_stride_logger(std::move(logger))
{
        assert(!m_config_pt.empty() && "PopBuilder::PopBuilder> Empty config ptree not acceptable!");
        // So as not to have to guard all log statements
        if (!m_stride_logger) {
                m_stride_logger = LogUtils::CreateNullLogger("PopBuilderNullLogger");
        }
}

void PopBuilder::MakePoolSys()
{
        using namespace ContactPoolType;
        auto& population = *m_pop;
        auto& poolSys    = population.GetContactPoolSys();

        // --------------------------------------------------------------
        // Determine number of contact pools from ids in population.
        // --------------------------------------------------------------
        IdSubscriptArray<unsigned int> max_ids{0U};
        for (const auto& p : population) {
                for (Id typ : IdList) {
                        max_ids[typ] = max(max_ids[typ], p.GetPoolId(typ));
                }
        }
        // --------------------------------------------------------------
        // Keep separate id counter to provide a unique id for every
        // contactpool. Start at 1 (see next item for pool_id==0).
        // --------------------------------------------------------------
        unsigned int id = 1;
        for (Id typ : IdList) {
                for (size_t i = 0; i <= max_ids[typ]; i++) {
                        poolSys[typ].emplace_back(ContactPool(id, typ));
                        id++;
                }
        }
        // --------------------------------------------------------------
        // Insert persons (pointers) in their contactpools. Having
        // contactpool id '0' means "not belonging pool of that type"
        // (school / work - belong to both).
        // --------------------------------------------------------------
        for (auto& p : population) {
                for (Id typ : IdList) {
                        const auto poolId = p.GetPoolId(typ);
                        if (poolId > 0) {
                                poolSys[typ][poolId].AddMember(&p);
                        }
                }
        }
        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
}

void PopBuilder::MakePersons()
{
        //------------------------------------------------
        // Read persosns from file.
        //------------------------------------------------
        const auto belief_pt        = m_config_pt.get_child("run.belief_policy");
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
                const auto risk_averseness        = (values.size() <= 6) ? 0.0 : FromString<double>(values[6]);
                const auto age                    = FromString<unsigned int>(values[0]);
                const auto household_id           = FromString<unsigned int>(values[1]);
                const auto school_id              = FromString<unsigned int>(values[2]);
                const auto work_id                = FromString<unsigned int>(values[3]);
                const auto primary_community_id   = FromString<unsigned int>(values[4]);
                const auto secondary_community_id = FromString<unsigned int>(values[5]);

                m_pop->CreatePerson(person_id, age, household_id, school_id, work_id, primary_community_id,
                                    secondary_community_id, Health(), belief_pt, risk_averseness);
                ++person_id;
        }

        pop_file.close();
}

std::shared_ptr<Population> PopBuilder::Build()
{
        //---------------------------------------------------------------
        // Preliminaries (check input data, rnManager, contactLogger).
        //---------------------------------------------------------------
        m_stride_logger->trace("Starting Preliminaries.");
        Preliminaries();
        m_stride_logger->trace("Finished Preliminaries.");

        // --------------------------------------------------------------
        // Read persons from file.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting MakePersons.");
        MakePersons();
        m_stride_logger->trace("Finished MakePersons.");

        // --------------------------------------------------------------
        // Fill up the various type of contactpools.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting MakePoolSys.");
        MakePoolSys();
        m_stride_logger->trace("Finished MakePoolSys.");

        // --------------------------------------------------------------
        // Seed the population with social contact survey participants.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting SurveySeeder.");
        SurveySeeder::Seed(m_config_pt, m_pop, m_rn_manager);
        m_stride_logger->trace("Finished SurveySeeder.");

        //------------------------------------------------
        // Done
        //------------------------------------------------
        return m_pop;
}

void PopBuilder::Preliminaries()
{
        //------------------------------------------------
        // Check validity of input data.
        //------------------------------------------------
        const auto seeding_rate = m_config_pt.get<double>("run.seeding_rate");
        if (seeding_rate > 1.0) {
                throw runtime_error(string(__func__) + "> Bad input data for seeding_rate.");
        }

        // ------------------------------------------------
        // Setup RNManager.
        // ------------------------------------------------
        m_num_threads = m_config_pt.get<unsigned int>("run.num_threads");
        m_rn_manager.Initialize(RNManager::Info{m_config_pt.get<string>("pop.rng_type", "lcg64"),
                                                m_config_pt.get<unsigned long>("run.rng_seed", 101UL), "",
                                                m_num_threads});

        // -----------------------------------------------------------------------------------------
        // Create contact_logger to log contacts/transmissions. Do NOT register it.
        // -----------------------------------------------------------------------------------------
        if (m_config_pt.get<bool>("run.contact_output_file", true)) {
                const auto prefix         = m_config_pt.get<string>("run.output_prefix");
                const auto logPath        = FileSys::BuildPath(prefix, "contact_log.txt");
                m_pop->GetContactLogger() = LogUtils::CreateRotatingLogger("contact_logger", logPath.string());
                // Remove meta data from log => time-stamp of logging
                m_pop->GetContactLogger()->set_pattern("%v");
        } else {
                m_pop->GetContactLogger() = LogUtils::CreateNullLogger("contact_logger");
        }
}

} // namespace stride
