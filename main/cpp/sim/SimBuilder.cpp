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

#include "contact/ContactType.h"
#include "contact/InfectorMap.h"
#include "disease/DiseaseSeeder.h"
#include "disease/HealthSeeder.h"
#include "pop/SurveySeeder.h"
#include "sim/Sim.h"
#include "util/FileSys.h"
#include "util/Rn.h"

#include <trng/uniform01_dist.hpp>

namespace stride {

using namespace boost::property_tree;
using namespace std;
using namespace util;
using namespace ContactType;

SimBuilder::SimBuilder(const ptree& configPt) : m_config(configPt) {}

shared_ptr<Sim> SimBuilder::Build(shared_ptr<Sim> sim, shared_ptr<Population> pop)
{
        // --------------------------------------------------------------
        // Read config info and setup random number manager
        // --------------------------------------------------------------
        sim->m_config                        = m_config;
        sim->m_population                    = std::move(pop);
        sim->m_track_index_case              = m_config.get<bool>("run.track_index_case");
        sim->m_adaptive_symptomatic_behavior = m_config.get<bool>("run.adaptive_symptomatic_behavior", true);
        sim->m_num_threads                   = m_config.get<unsigned int>("run.num_threads");
        sim->m_calendar                      = make_shared<Calendar>(m_config);
        sim->m_contact_log_mode = ContactLogMode::ToMode(m_config.get<string>("run.contact_log_level", "None"));

        // TODO this ought to be redundant and on mac it is, but on linux python scripts crash if it ins't there
        sim->m_rn_manager.GetInfo();

        // --------------------------------------------------------------
        // Contact handlers, each with generator bound to different
        // random engine stream) and infector.
        // --------------------------------------------------------------
        for (auto i = 0U; i < sim->m_num_threads; i++) {
                auto gen = sim->m_rn_manager[i].variate_generator(trng::uniform01_dist<double>());
                sim->m_handlers.emplace_back(ContactHandler(gen));
        }
        const auto& select = make_tuple(sim->m_contact_log_mode, sim->m_track_index_case);
        sim->m_infector    = InfectorMap().at(select);

        // --------------------------------------------------------------
        // Initialize the age-related contact profiles.
        // --------------------------------------------------------------
        const auto ageContactPt = ReadAgeContactPtree();
        for (Id typ : IdList) {
                sim->m_contact_profiles[typ] = AgeContactProfile(typ, ageContactPt);
        }

        // --------------------------------------------------------------
        // Initialize the transmission profile (fixes rates).
        // --------------------------------------------------------------
        const auto diseasePt = ReadDiseasePtree();
        sim->m_transmission_profile.Initialize(m_config, diseasePt);

        // --------------------------------------------------------------
        // Initialize the public health agency (fixes detection probability).
        // --------------------------------------------------------------
        const double detection_probability = m_config.get<double>("run.case_detection_probability", 0.0);
        sim->m_public_health_agency.Initialize(detection_probability);

        // --------------------------------------------------------------
        // Seed the population with health data.
        // --------------------------------------------------------------
        HealthSeeder(diseasePt).Seed(sim->m_population, sim->m_handlers);

        // --------------------------------------------------------------
        // Seed population with immunity/vaccination/infection.
        // --------------------------------------------------------------
        DiseaseSeeder(m_config, sim->m_rn_manager).Seed(sim->m_population);

        // --------------------------------------------------------------
        // Seed population with survey participants.
        // --------------------------------------------------------------
        SurveySeeder(m_config, sim->m_rn_manager).Seed(sim->m_population);

        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        return sim;
}

ptree SimBuilder::ReadAgeContactPtree()
{
        const auto fn = m_config.get<string>("run.age_contact_matrix_file", "contact_matrix.xml");
        const auto fp = m_config.get<bool>("run.use_install_dirs") ? FileSys::GetDataDir() /= fn : filesys::path(fn);
        return FileSys::ReadPtreeFile(fp);
}

ptree SimBuilder::ReadDiseasePtree()
{
        const auto fn = m_config.get<string>("run.disease_config_file");
        const auto fp = m_config.get<bool>("run.use_install_dirs") ? FileSys::GetDataDir() /= fn : filesys::path(fn);
        return FileSys::ReadPtreeFile(fp);
}

} // namespace stride
