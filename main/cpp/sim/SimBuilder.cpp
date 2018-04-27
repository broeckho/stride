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
#include "contact/InfectorMap.h"
#include "disease/DiseaseSeeder.h"
#include "disease/HealthSeeder.h"
#include "pool/ContactPoolType.h"
#include "pop/PopBuilder.h"
#include "sim/Sim.h"
#include "util/FileSys.h"
#include "util/LogUtils.h"

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <trng/uniform01_dist.hpp>

namespace stride {

using namespace boost::property_tree;
using namespace boost::filesystem;
using namespace std;
using namespace util;
using namespace ContactPoolType;

SimBuilder::SimBuilder(const ptree& configPt) : m_config_pt(configPt) {}

SimBuilder::SimBuilder(const std::string& configString)
{
	// Create ptree from xml string
	stringstream ss;
	ss << configString;
	read_xml(ss, m_config_pt, xml_parser::trim_whitespace);
}

std::shared_ptr<Sim> SimBuilder::Build()
{
        // --------------------------------------------------------------
        // Preliminaries.
        // --------------------------------------------------------------
        struct make_shared_enabler : public Sim
        {
        };
        shared_ptr<Sim> sim          = make_shared<make_shared_enabler>();
        const auto      diseasePt    = ReadDiseasePtree();
        const auto      ageContactPt = ReadAgeContactPtree();

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
        // Random number manager.
        // --------------------------------------------------------------
        sim->m_rn_manager.Initialize(RNManager::Info{m_config_pt.get<string>("run.rng_type", "mrg2"),
                                                     m_config_pt.get<unsigned long>("run.rng_seed", 1UL), "",
                                                     sim->m_num_threads});

        // --------------------------------------------------------------
        // Contact handlers, each with generator bound to different
        // random engine stream) and infector.
        // --------------------------------------------------------------
        for (size_t i = 0; i < sim->m_num_threads; i++) {
                auto gen = sim->m_rn_manager.GetGenerator(trng::uniform01_dist<double>(), i);
                sim->m_handlers.emplace_back(ContactHandler(gen));
        }

        const auto& select = make_tuple(sim->m_contact_log_mode, sim->m_track_index_case, sim->m_local_info_policy);
        sim->m_infector    = InfectorMap().at(select);

        // --------------------------------------------------------------
        // Build population.
        // --------------------------------------------------------------
        sim->m_population = PopBuilder(m_config_pt).Build();

        // --------------------------------------------------------------
        // Seed the population with health data.
        // --------------------------------------------------------------
        HealthSeeder(diseasePt, sim->m_rn_manager).Seed(sim->m_population);

        // --------------------------------------------------------------
        // Initialize the age-related contact profiles.
        // --------------------------------------------------------------
        for (Id typ : IdList) {
                sim->m_contact_profiles[typ] = AgeContactProfile(typ, ageContactPt);
        }

        // --------------------------------------------------------------
        // Initialize the transmission profile (fixes rates).
        // --------------------------------------------------------------
        sim->m_transmission_profile.Initialize(m_config_pt, diseasePt);

        // --------------------------------------------------------------
        // Seed population wrt immunity/vaccination/infection.
        // --------------------------------------------------------------
        DiseaseSeeder(m_config_pt, sim->m_rn_manager).Seed(sim->m_population);

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
                throw runtime_error("SimBuilder::ReadAgeContactPtree> Not finding " + fp.string());
        } else {
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
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
                throw runtime_error("SimBuilder::ReadDiseasePtree> Not finding " + fp.string());
        } else {
                try {
                        read_xml(canonical(fp).string(), pt, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        throw runtime_error("SimBuilder::ReadDiseasePtree> Error reading " + fp.string());
                }
        }
        return pt;
}

} // namespace stride
