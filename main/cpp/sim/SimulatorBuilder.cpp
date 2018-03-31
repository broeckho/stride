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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Implementation for the SimulatorBuilder class.
 */

#include "SimulatorBuilder.h"

#include "calendar/Calendar.h"
#include "disease/DiseaseSeeder.h"
#include "disease/HealthSeeder.h"
#include "pool/ContactPoolType.h"
#include "pop/PopPoolBuilder.h"
#include "pop/PopulationBuilder.h"
#include "pop/SurveySeeder.h"
#include "sim/Simulator.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <trng/uniform_int_dist.hpp>
#include <cassert>

namespace stride {

using namespace boost::property_tree;
using namespace boost::filesystem;
using namespace std;
using namespace util;
using namespace ContactPoolType;

SimulatorBuilder::SimulatorBuilder(const boost::property_tree::ptree& config_pt, std::shared_ptr<spdlog::logger> logger)
    : m_config_pt(config_pt), m_stride_logger(std::move(logger))
{
        assert(m_stride_logger && "SimulatorBuilder::SimulatorBuilder> Nullptr not acceptable!");
        assert(m_config_pt.empty() && "SimulatorBuilder::SimulatorBuilder> Empty ptree not acceptable!");
        // So as not to have to guard all log statements
        if (!m_stride_logger) {
                m_stride_logger = LogUtils::CreateNullLogger("SimulatorBuilder_Null_Logger");
        }
}

std::shared_ptr<Simulator> SimulatorBuilder::Build()
{
        m_stride_logger->trace("Starting SimulatorBuilder::Build.");
        const auto pt_contact = ReadContactPtree();
        const auto pt_disease = ReadDiseasePtree();

        std::shared_ptr<Simulator> sim = nullptr;
        if (!pt_contact.empty() && !pt_disease.empty()) {
                sim = Build(pt_disease, pt_contact);
        }

        m_stride_logger->trace("Finished SimulatorBuilder::Build.");
        return sim;
}

ptree SimulatorBuilder::ReadContactPtree()
{
        const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");

        ptree      pt;
        const auto fn = m_config_pt.get("run.age_contact_matrix_file", "contact_matrix.xml");
        const auto fp = (use_install_dirs) ? FileSys::GetDataDir() /= fn : fn;
        if (!exists(fp) || !is_regular_file(fp)) {
                m_stride_logger->critical("Age-Contact matrix file {} not present! Quitting.", fp.string());
        } else {
                m_stride_logger->debug("Age-Contact matrix file:  {}", fp.string());
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        m_stride_logger->critical("Error reading {}\nException: {}", canonical(fp).string(), e.what());
                        pt.clear();
                }
        }

        return pt;
}

ptree SimulatorBuilder::ReadDiseasePtree()
{
        const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");

        ptree      pt;
        const auto fn = m_config_pt.get<string>("run.disease_config_file");
        const auto fp = (use_install_dirs) ? FileSys::GetDataDir() /= fn : fn;
        if (!exists(fp) || !is_regular_file(fp)) {
                m_stride_logger->critical("Disease config file {} not present! Quitting.", fp.string());
        } else {
                m_stride_logger->debug("Disease config file:  {}", fp.string());
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        m_stride_logger->critical("Error reading {}\nException: {}", canonical(fp).string(), e.what());
                        pt.clear();
                }
        }

        return pt;
}

std::shared_ptr<Simulator> SimulatorBuilder::Build(const ptree& disease_pt, const ptree& contact_pt)
{
        // --------------------------------------------------------------
        // Uninitialized simulator object.
        // --------------------------------------------------------------
        auto sim = make_shared<Simulator>();

        // --------------------------------------------------------------
        // Config info.
        // --------------------------------------------------------------
        sim->m_config_pt        = m_config_pt;
        sim->m_track_index_case = m_config_pt.get<bool>("run.track_index_case");
        sim->m_num_threads      = m_config_pt.get<unsigned int>("run.num_threads");
        sim->m_calendar         = make_shared<Calendar>(m_config_pt);

        // --------------------------------------------------------------
        // Initialize RNManager for random number engine management.
        // --------------------------------------------------------------
        const auto            rng_seed = m_config_pt.get<unsigned long>("run.rng_seed", 1UL);
        const auto            rng_type = m_config_pt.get<string>("run.rng_type", "mrg2");
        const RNManager::Info info{rng_type, rng_seed, "", sim->m_num_threads};
        sim->m_rn_manager.Initialize(info);

        // --------------------------------------------------------------
        // ContactLogMode related initialization.
        // --------------------------------------------------------------
        const string l          = m_config_pt.get<string>("run.contact_log_level", "None");
        sim->m_contact_log_mode = ContactLogMode::IsMode(l)
                                      ? ContactLogMode::ToMode(l)
                                      : throw runtime_error(string(__func__) + "> Invalid input for ContactLogMode.");

        // -----------------------------------------------------------------------------------------
        // Create contact_logger for the simulator to log contacts/transmissions. Do NOT register it.
        // Transmissions: [TRANSMISSION] <infecterID> <infectedID> <contactpoolID> <day>
        // Contacts: [CNT] <person1ID> <person1AGE> <person2AGE> <at_home> <at_work> <at_school> <at_other>
        // -----------------------------------------------------------------------------------------
        const auto contact_output_file = m_config_pt.get<bool>("run.contact_output_file", true);
        if (contact_output_file) {
                const auto output_prefix = m_config_pt.get<string>("run.output_prefix");
                const auto log_path      = FileSys::BuildPath(output_prefix, "contact_log.txt");
                sim->m_contact_logger    = LogUtils::CreateRotatingLogger("contact_logger", log_path.string());
                // Remove meta data from log => time-stamp of logging
                sim->m_contact_logger->set_pattern("%v");
        } else {
                sim->m_contact_logger = LogUtils::CreateNullLogger("contact_logger");
        }

        // --------------------------------------------------------------
        // Set correct information policies.
        // --------------------------------------------------------------
        const string loc_info_policy    = m_config_pt.get<string>("run.local_information_policy", "NoLocalInformation");
        sim->m_local_information_policy = loc_info_policy;

        // --------------------------------------------------------------
        // Build population.
        // --------------------------------------------------------------
        sim->m_population = PopulationBuilder::Build(m_config_pt, sim->m_rn_manager);

        // --------------------------------------------------------------
        // Seed the population with social contact survey participants.
        // --------------------------------------------------------------
        SurveySeeder::Seed(m_config_pt, sim->m_population, sim->m_rn_manager, sim->m_contact_logger);

        // --------------------------------------------------------------
        // Seed the population with health data.
        // --------------------------------------------------------------
        HealthSeeder h_seeder(disease_pt, sim->m_rn_manager);
        h_seeder.Seed(sim->m_population);

        // --------------------------------------------------------------
        // Initialize the age-related contact profiles.
        // --------------------------------------------------------------
        for (Id typ : IdList) {
                sim->m_contact_profiles[typ] = AgeContactProfile(typ, contact_pt);
        }

        // --------------------------------------------------------------
        // Build the ContactPoolSystem of the simulator.
        // --------------------------------------------------------------
        PopPoolBuilder cp_builder(m_stride_logger);
        cp_builder.Build(sim->m_pool_sys, *sim->m_population);

        // --------------------------------------------------------------
        // Initialize the transmission profile (fixes rates).
        // --------------------------------------------------------------
        sim->m_operational = sim->m_disease_profile.Initialize(m_config_pt, disease_pt);

        // --------------------------------------------------------------
        // Seed population wrt immunity/vaccination/infection.
        // --------------------------------------------------------------
        DiseaseSeeder d_builder(m_config_pt, sim->m_rn_manager);
        d_builder.Seed(sim);

        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        return sim;
}

} // namespace stride
