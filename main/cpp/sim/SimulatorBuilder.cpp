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

#include "core/ClusterType.h"
#include "immunity/Vaccinator.h"
#include "pop/PopulationBuilder.h"
#include "util/InstallDirs.h"

#include <boost/property_tree/xml_parser.hpp>
#include <spdlog/spdlog.h>

namespace stride {

using namespace boost::property_tree;
using namespace std;
using namespace util;

///
std::shared_ptr<Simulator> SimulatorBuilder::Build(const std::string& config_file_name, unsigned int num_threads,
						   bool track_index_case)
{
	ptree pt_config;
	const auto file_path = InstallDirs::GetCurrentDir() /= config_file_name;
	if (!is_regular_file(file_path)) {
		throw runtime_error(string(__func__) + ">Config file " + file_path.string() +
				    " not present. Aborting.");
	}
	read_xml(file_path.string(), pt_config);
	return Build(pt_config, num_threads, track_index_case);
}

/// Build the simulator.
std::shared_ptr<Simulator> SimulatorBuilder::Build(const boost::property_tree::ptree& pt_config,
						   unsigned int num_threads, bool track_index_case)
{
	ptree pt_disease;

	const auto file_name_d{pt_config.get<string>("run.disease_config_file")};
	const auto file_path_d{InstallDirs::GetDataDir() /= file_name_d};
	if (!is_regular_file(file_path_d)) {
		throw runtime_error(std::string(__func__) + "> No file " + file_path_d.string());
	}
	read_xml(file_path_d.string(), pt_disease);

	// Contact file.
	ptree pt_contact;
	const auto file_name_c{pt_config.get("run.age_contact_matrix_file", "contact_matrix.xml")};
	const auto file_path_c{InstallDirs::GetDataDir() /= file_name_c};
	if (!is_regular_file(file_path_c)) {
		throw runtime_error(string(__func__) + "> No file " + file_path_c.string());
	}
	read_xml(file_path_c.string(), pt_contact);

	return Build(pt_config, pt_disease, pt_contact, num_threads, track_index_case);
}

/// Build the simulator.
std::shared_ptr<Simulator> SimulatorBuilder::Build(const boost::property_tree::ptree& pt_config,
						   const boost::property_tree::ptree& pt_disease,
						   const boost::property_tree::ptree& pt_contact,
						   unsigned int num_threads, bool track_index_case)
{
	auto sim = make_shared<Simulator>();
	const shared_ptr<spdlog::logger> logger = spdlog::get("contact_logger");

	sim->m_pt_config = pt_config;                       // Initialize config ptree.
	sim->m_track_index_case = track_index_case;         // Initialize track_index_case policy
	sim->m_num_threads = num_threads;                   // Initialize number of threads.
	sim->m_calendar = make_shared<Calendar>(pt_config); // Initialize calendar.

	// Get log level.
	const string l = pt_config.get<string>("run.log_level", "None");
	sim->m_log_level =
	    IsLogMode(l) ? ToLogMode(l) : throw runtime_error(string(__func__) + "> Invalid input for LogMode.");

	/// Set correct information policies
	const string loc_info_policy = pt_config.get<string>("run.local_information_policy", "NoLocalInformation");
	sim->m_local_information_policy = loc_info_policy; // TODO make this enum class like LogMode

	// Build population.
	Random rng(pt_config.get<double>("run.rng_seed"));
	sim->m_population = PopulationBuilder::Build(pt_config, pt_disease, rng);

	// Initialize clusters.
	InitializeClusters(sim);

	// Initialize population immunity
	Vaccinator::Apply("immunity", sim, pt_config, pt_disease, rng);
	Vaccinator::Apply("vaccine", sim, pt_config, pt_disease, rng);

	// Initialize disease profile.
	sim->m_disease_profile.Initialize(pt_config, pt_disease);

	// --------------------------------------------------------------
	// Seed infected persons.
	// --------------------------------------------------------------
	const double seeding_rate = pt_config.get<double>("run.seeding_rate");
	const double seeding_age_min = pt_config.get<double>("run.seeding_age_min", 1);
	const double seeding_age_max = pt_config.get<double>("run.seeding_age_max", 99);
	const unsigned int max_population_index = sim->m_population->size() - 1;
	unsigned int num_infected = floor(static_cast<double>(sim->m_population->size()) * seeding_rate);
	while (num_infected > 0) {
		Person& p = sim->m_population->at(rng(max_population_index));
		if (p.GetHealth().IsSusceptible() && (p.GetAge() >= seeding_age_min) &&
		    (p.GetAge() <= seeding_age_max)) {
			p.GetHealth().StartInfection();
			num_infected--;
			logger->info("[PRIM] {} {} {} {}", -1, p.GetId(), -1, 0);
		}
	}

	// Initialize Rng handlers
	unsigned int new_seed = rng(numeric_limits<unsigned int>::max());
	for (size_t i = 0; i < sim->m_num_threads; i++) {
		sim->m_rng_handler.emplace_back(RngHandler(new_seed, sim->m_num_threads, i));
	}

	// Initialize contact profiles.
	Cluster::AddContactProfile(ClusterType::Household, ContactProfile(ClusterType::Household, pt_contact));
	Cluster::AddContactProfile(ClusterType::School, ContactProfile(ClusterType::School, pt_contact));
	Cluster::AddContactProfile(ClusterType::Work, ContactProfile(ClusterType::Work, pt_contact));
	Cluster::AddContactProfile(ClusterType::PrimaryCommunity,
				   ContactProfile(ClusterType::PrimaryCommunity, pt_contact));
	Cluster::AddContactProfile(ClusterType::SecondaryCommunity,
				   ContactProfile(ClusterType::SecondaryCommunity, pt_contact));

	// Done.
	return sim;
}

/// Initialize the clusters.
void SimulatorBuilder::InitializeClusters(std::shared_ptr<Simulator> sim)
{
	// Determine the number of clusters.
	unsigned int max_id_households{0U};
	unsigned int max_id_school_clusters{0U};
	unsigned int max_id_work_clusters{0U};
	unsigned int max_id_primary_community{0U};
	unsigned int max_id_secondary_community{0U};

	Population& population{*sim->m_population};

	for (const auto& p : population) {
		max_id_households = std::max(max_id_households, p.GetClusterId(ClusterType::Household));
		max_id_school_clusters = std::max(max_id_school_clusters, p.GetClusterId(ClusterType::School));
		max_id_work_clusters = std::max(max_id_work_clusters, p.GetClusterId(ClusterType::Work));
		max_id_primary_community =
		    std::max(max_id_primary_community, p.GetClusterId(ClusterType::PrimaryCommunity));
		max_id_secondary_community =
		    std::max(max_id_secondary_community, p.GetClusterId(ClusterType::SecondaryCommunity));
	}

	// Keep separate id counter to provide a unique id for every cluster.
	unsigned int c_id = 1;

	for (size_t i = 0; i <= max_id_households; i++) {
		sim->m_households.emplace_back(Cluster(c_id, ClusterType::Household));
		c_id++;
	}
	for (size_t i = 0; i <= max_id_school_clusters; i++) {
		sim->m_school_clusters.emplace_back(Cluster(c_id, ClusterType::School));
		c_id++;
	}
	for (size_t i = 0; i <= max_id_work_clusters; i++) {
		sim->m_work_clusters.emplace_back(Cluster(c_id, ClusterType::Work));
		c_id++;
	}
	for (size_t i = 0; i <= max_id_primary_community; i++) {
		sim->m_primary_community.emplace_back(Cluster(c_id, ClusterType::PrimaryCommunity));
		c_id++;
	}
	for (size_t i = 0; i <= max_id_secondary_community; i++) {
		sim->m_secondary_community.emplace_back(Cluster(c_id, ClusterType::SecondaryCommunity));
		c_id++;
	}

	// Cluster id '0' means "not present in any cluster of that type".
	for (auto& p : population) {
		const auto hh_id = p.GetClusterId(ClusterType::Household);
		if (hh_id > 0) {
			sim->m_households[hh_id].AddPerson(&p);
		}
		const auto sc_id = p.GetClusterId(ClusterType::School);
		if (sc_id > 0) {
			sim->m_school_clusters[sc_id].AddPerson(&p);
		}
		const auto wo_id = p.GetClusterId(ClusterType::Work);
		if (wo_id > 0) {
			sim->m_work_clusters[wo_id].AddPerson(&p);
		}
		const auto primCom_id = p.GetClusterId(ClusterType::PrimaryCommunity);
		if (primCom_id > 0) {
			sim->m_primary_community[primCom_id].AddPerson(&p);
		}
		const auto secCom_id = p.GetClusterId(ClusterType::SecondaryCommunity);
		if (secCom_id > 0) {
			sim->m_secondary_community[secCom_id].AddPerson(&p);
		}
	}
}

} // end_of_namespace
