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

#include "behaviour/BeliefSeeder.h"
#include "contact/InfectorMap.h"
#include "disease/DiseaseSeeder.h"
#include "disease/HealthSeeder.h"
#include "pool/ContactPoolType.h"
#include "sim/Sim.h"
#include "util/FileSys.h"

#include <trng/uniform01_dist.hpp>

namespace stride {

using namespace boost::property_tree;
using namespace boost::filesystem;
using namespace std;
using namespace util;
using namespace ContactPoolType;

SimBuilder::SimBuilder(const ptree& configPt) : m_config_pt(configPt) {}

shared_ptr<Sim> SimBuilder::Build(shared_ptr<Sim> sim, shared_ptr<Population> pop)
{
        // --------------------------------------------------------------
        // Read config info and setup random number manager
        // --------------------------------------------------------------
        sim->m_config_pt         = m_config_pt;
        sim->m_population        = std::move(pop);
        sim->m_track_index_case  = m_config_pt.get<bool>("run.track_index_case");
        sim->m_num_threads       = m_config_pt.get<unsigned int>("run.num_threads");
        sim->m_calendar          = make_shared<Calendar>(m_config_pt);
        sim->m_local_info_policy = m_config_pt.get<string>("run.local_information_policy", "NoLocalInformation");
        sim->m_contact_log_mode  = ContactLogMode::ToMode(m_config_pt.get<string>("run.contact_log_level", "None"));
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
        sim->m_transmission_profile.Initialize(m_config_pt, diseasePt);

        // --------------------------------------------------------------
        // Seed the population with health data.
        // --------------------------------------------------------------
        HealthSeeder(diseasePt).Seed(sim->m_population, sim->m_handlers);

        // --------------------------------------------------------------
        // Seed population wrt immunity/vaccination/infection.
        // --------------------------------------------------------------
        DiseaseSeeder(m_config_pt, sim->m_rn_manager).Seed(sim->m_population);

        // --------------------------------------------------------------
        // Seed population wrt belief policies.
        // --------------------------------------------------------------
        BeliefSeeder(m_config_pt, sim->m_rn_manager).Seed(sim->m_population);

        // --------------------------------------------------------------
        // Done.
        // --------------------------------------------------------------
        return sim;
}

ptree SimBuilder::ReadAgeContactPtree()
{
        const auto fn = m_config_pt.get<string>("run.age_contact_matrix_file", "contact_matrix.xml");
        const auto fp = m_config_pt.get<bool>("run.use_install_dirs") ? FileSys::GetDataDir() /= fn : fn;
        return FileSys::ReadPtreeFile(fp);
}

ptree SimBuilder::ReadDiseasePtree()
{
        const auto fn = m_config_pt.get<string>("run.disease_config_file");
        const auto fp = m_config_pt.get<bool>("run.use_install_dirs") ? FileSys::GetDataDir() /= fn : fn;
        return FileSys::ReadPtreeFile(fp);
}

} // namespace stride
