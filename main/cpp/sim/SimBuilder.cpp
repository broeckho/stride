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
 * Implementation for the SimBuilder class.
 */

#include "SimBuilder.h"

#include "calendar/Calendar.h"
#include "disease/DiseaseSeeder.h"
#include "disease/HealthSeeder.h"
#include "pool/ContactPoolType.h"
#include "pop/PopBuilder.h"
#include "sim/Sim.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace stride {

using namespace boost::property_tree;
using namespace boost::filesystem;
using namespace std;
using namespace util;
using namespace ContactPoolType;

SimBuilder::SimBuilder(const ptree& configPt, std::shared_ptr<spdlog::logger> logger)
    : m_config_pt(configPt), m_stride_logger(std::move(logger))
{
        // So as not to have to guard all log statements
        if (!m_stride_logger) {
                m_stride_logger = LogUtils::CreateNullLogger("NullLogger");
        }
}

std::shared_ptr<Sim> SimBuilder::Build()
{
        const auto contactPt = ReadAgeContactPtree();
        const auto diseasePt = ReadDiseasePtree();
        auto       sim       = Build(diseasePt, contactPt);
        return sim;
}

std::shared_ptr<Sim> SimBuilder::Build(const ptree& diseasePt, const ptree& ageContactPt)
{
        // --------------------------------------------------------------
        // Uninitialized simulator object.
        // --------------------------------------------------------------
        auto sim = make_shared<Sim>();

        // --------------------------------------------------------------
        // Config info.
        // --------------------------------------------------------------
        sim->m_config_pt         = m_config_pt;
        sim->m_track_index_case  = m_config_pt.get<bool>("run.track_index_case");
        sim->m_num_threads       = m_config_pt.get<unsigned int>("run.num_threads");
        sim->m_calendar          = make_shared<Calendar>(m_config_pt);
        sim->m_local_info_policy = m_config_pt.get<string>("run.local_information_policy", "NoLocalInformation");
        sim->m_contact_log_mode  = ContactLogMode::ToMode(m_config_pt.get<string>("run.contact_log_level", "None"));

        // --------------------------------------------------------------
        // Initialize RNManager for random number engine management.
        // --------------------------------------------------------------
        m_stride_logger->trace("Initializing random number manager.");
        sim->m_rn_manager.Initialize(RNManager::Info{m_config_pt.get<string>("run.rng_type", "mrg2"),
                                                     m_config_pt.get<unsigned long>("run.rng_seed", 1UL), "",
                                                     sim->m_num_threads});
        m_stride_logger->trace("Done Initializing random number manager.");

        // --------------------------------------------------------------
        // Build population.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting PopBuilder.");
        sim->m_population = PopBuilder(m_config_pt, m_stride_logger).Build();
        m_stride_logger->trace("Finished PopBuilder.");

        // --------------------------------------------------------------
        // Seed the population with health data.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting HealthSeeder.");
        HealthSeeder(diseasePt, sim->m_rn_manager).Seed(sim->m_population);
        m_stride_logger->trace("Finished HealthSeeder.");

        // --------------------------------------------------------------
        // Initialize the age-related contact profiles.
        // --------------------------------------------------------------
        m_stride_logger->trace("Initializing Age-Contact profiles.");
        for (Id typ : IdList) {
                sim->m_contact_profiles[typ] = AgeContactProfile(typ, ageContactPt);
        }
        m_stride_logger->trace("Done initializing Age-Contact profiles.");

        // --------------------------------------------------------------
        // Initialize the transmission profile (fixes rates).
        // --------------------------------------------------------------
        m_stride_logger->trace("Initializing Transmission profiles.");
        sim->m_transmission_profile.Initialize(m_config_pt, diseasePt);
        m_stride_logger->trace("Done initializing Transmission profiles.");

        // --------------------------------------------------------------
        // Seed population wrt immunity/vaccination/infection.
        // --------------------------------------------------------------
        m_stride_logger->trace("Starting DiseaseSeeder.");
        DiseaseSeeder(m_config_pt, sim->m_rn_manager).Seed(sim->m_population);
        m_stride_logger->trace("Finished DiseaseSeeder.");

        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        return sim;
}

ptree SimBuilder::ReadAgeContactPtree()
{
        const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");

        ptree      pt;
        const auto fn = m_config_pt.get("run.age_contact_matrix_file", "contact_matrix.xml");
        const auto fp = (use_install_dirs) ? FileSys::GetDataDir() /= fn : fn;
        if (!exists(fp) || !is_regular_file(fp)) {
                m_stride_logger->critical("Age-Contact matrix file {} not present! Quitting.", fp.string());
                throw runtime_error("SimBuilder::ReadAgeContactPtree> Not finding " + fp.string());
        } else {
                m_stride_logger->debug("Age-Contact matrix file:  {}", fp.string());
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        m_stride_logger->critical("Error reading {}\nException: {}", canonical(fp).string(), e.what());
                        throw runtime_error("SimBuilder::ReadAgeContactPtree> Error reading " + fp.string());
                }
        }

        return pt;
}

ptree SimBuilder::ReadDiseasePtree()
{
        const auto use_install_dirs = m_config_pt.get<bool>("run.use_install_dirs");

        ptree      pt;
        const auto fn = m_config_pt.get<string>("run.disease_config_file");
        const auto fp = (use_install_dirs) ? FileSys::GetDataDir() /= fn : fn;
        if (!exists(fp) || !is_regular_file(fp)) {
                m_stride_logger->critical("Disease config file {} not present! Quitting.", fp.string());
                throw runtime_error("SimBuilder::ReadDiseasePtree> Not finding " + fp.string());
        } else {
                m_stride_logger->debug("Disease config file:  {}", fp.string());
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        m_stride_logger->critical("Error reading {}\nException: {}", canonical(fp).string(), e.what());
                        throw runtime_error("SimBuilder::ReadDiseasePtree> Error reading " + fp.string());
                }
        }

        return pt;
}

} // namespace stride
