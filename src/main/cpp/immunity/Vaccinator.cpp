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
 * Implementation for the Vaccinator class.
 */

#include "Vaccinator.h"

#include "util/PtreeUtils.h"
#include "util/StringUtils.h"

namespace stride {

using namespace std;
using namespace util;

/// Initiate the given immunity distribution in the population, according the given link probability
void Vaccinator::Administer(std::vector<Cluster>* clusters,
		std::vector<double>& immunity_distribution,
		double immunity_link_probability, util::Random& rng) {
	// initiate a vector to count the population per age class [0-100]
	vector<double> population_count_age;
	for (unsigned int index_age = 0; index_age < 100; index_age++) {
		population_count_age.push_back(0);
	}

	// count individuals per age class and set all "susceptible" individuals "immune"
	// note: focusing on measles, we expect the number of susceptible individuals to be less compared to the
	// number of immune.
	// TODO but this is a generic simulator
	for (unsigned int i = 0; i < clusters->size(); i++) {
		Cluster& p_cluster = clusters->at(i);
		unsigned int household_size = p_cluster.GetSize();

		// loop over cluster members, in random order
		for (unsigned int i_p = 0; i_p < household_size; i_p++) {
			Person& p = *p_cluster.GetMember(i_p);
			if (p.GetHealth().IsSusceptible()) {
				p.GetHealth().SetImmune();
				population_count_age[p.GetAge()]++;
			}
		}
	}

	// calculate the number of susceptible individuals per age class
	unsigned int total_num_susceptible = 0;
	for (unsigned int index_age = 0; index_age < 100; index_age++) {
		population_count_age[index_age] = floor(
				population_count_age[index_age]
						* (1 - immunity_distribution[index_age]));
		total_num_susceptible += population_count_age[index_age];
	}

	// sample susceptible individuals, until all age-dependent quota are reached
	while (total_num_susceptible > 0) {
		// sample cluster
		Cluster& p_cluster = clusters->at(rng(clusters->size()));
		unsigned int household_size = p_cluster.GetSize();
		vector<unsigned int> member_indices = rng.GetRandomIndices(
				household_size);

		// loop over cluster members, in random order
		for (unsigned int i_p = 0;
				i_p < household_size && total_num_susceptible > 0; i_p++) {
			Person& p = *p_cluster.GetMember(member_indices[i_p]);

			// if individual is immune and his/her age class has not reached the quota => make
			// susceptible
			if (p.GetHealth().IsImmune()
					&& population_count_age[p.GetAge()] > 0) {
				p.GetHealth().SetSusceptible();
				population_count_age[p.GetAge()]--;
				total_num_susceptible--;
			}
			// random draw to continue in this cluster or to sample a new one
			if (rng.NextDouble() < (1 - immunity_link_probability)) {
				break;
			}
		}
	}
}

/// Administer cocoon immunization for the given rate and target ages [min-max] to protect connected
/// individuals of the given age class [min-max]
void Vaccinator::AdministerCocoon(std::vector<Cluster>* clusters, double immunity_rate,
		double adult_age_min, double adult_age_max, double child_age_min,
		double child_age_max, util::Random& rng) {
	for (unsigned int i = 0; i < clusters->size(); i++) {
		Cluster& p_cluster = clusters->at(i);

		// loop over cluster members
		for (unsigned int i_p = 0; i_p < p_cluster.GetSize(); i_p++) {
			Person& p = *p_cluster.GetMember(i_p);

			if (p.GetHealth().IsSusceptible() && p.GetAge() >= adult_age_min
					&& p.GetAge() <= adult_age_max) {

				bool is_connected_to_target_age = false;
				for (unsigned int i_p2 = 0;
						i_p2 < p_cluster.GetSize()
								&& !is_connected_to_target_age; i_p2++) {
					const Person& p2 = *p_cluster.GetMember(i_p2);
					if (p2.GetAge() >= child_age_min
							&& p2.GetAge() <= child_age_max) {
						is_connected_to_target_age = true;
					}
				}
				if (is_connected_to_target_age == true
						&& rng.NextDouble() < immunity_rate) {
					p.GetHealth().SetImmune();
				}
			}
		}
	}
}

/// Apply the immunization strategy in the configuration file to the Simulator object.
void Vaccinator::Apply(const std::string s, std::shared_ptr<Simulator> sim,
		const boost::property_tree::ptree& pt_config,
		const boost::property_tree::ptree& pt_disease, util::Random& rng) {
	const std::string immunity_profile = pt_config.get < string
			> ("run." + StringUtils::ToLower(s) + "_profile");
	std::cout << "Building: " << s << " profile => " << immunity_profile
			<< endl;

	if (immunity_profile == "None") {
		// do nothing
	} else {
		std::vector<double> immunity_distribution;

		// default
		std::vector<Cluster>* immunity_clusters = &sim->m_households;

		const double immunity_link_probability = (
				(immunity_profile == "Cocoon") ?
						1 :
						pt_config.get<double>(
								"run." + StringUtils::ToLower(s)
										+ "_link_probability"));

		if (immunity_link_probability > 0) {

			ClusterType immunity_link_clustertype = ToClusterType(
					pt_config.get < string
							> ("run." + StringUtils::ToLower(s)
									+ "_link_clustertype"));

			switch (immunity_link_clustertype) {
			case ClusterType::Household:
				immunity_clusters = &sim->m_households;
				break;
			case ClusterType::School:
				immunity_clusters = &sim->m_school_clusters;
				break;
			case ClusterType::Work:
				immunity_clusters = &sim->m_work_clusters;
				break;
			case ClusterType::PrimaryCommunity:
				immunity_clusters = &sim->m_primary_community;
				break;
			case ClusterType::SecondaryCommunity:
				immunity_clusters = &sim->m_secondary_community;
				break;
			default:
				throw runtime_error(
						std::string(__func__)
								+ "Link cluster type screwed up!");
			}
		}

		if (immunity_profile == "Cocoon") {
			std::cout << "cocoon" << std::endl;
			const double immunity_rate = pt_config.get<double>(
					"run." + StringUtils::ToLower(s) + "_rate");
			const double adult_age_min = pt_config.get<double>(
					"run." + StringUtils::ToLower(s) + "_adult_age_min");
			const double adult_age_max = pt_config.get<double>(
					"run." + StringUtils::ToLower(s) + "_adult_age_max");
			const double child_age_min = pt_config.get<double>(
					"run." + StringUtils::ToLower(s) + "_child_age_max");
			const double child_age_max = pt_config.get<double>(
					"run." + StringUtils::ToLower(s) + "_child_age_max");

			AdministerCocoon(immunity_clusters, immunity_rate, adult_age_min,
					adult_age_max, child_age_min, child_age_max, rng);
		} else {
			if (immunity_profile == "Random") {
				const double immunity_rate = pt_config.get<double>(
						"run." + StringUtils::ToLower(s) + "_rate");
				for (unsigned int index_age = 0; index_age < 100; index_age++) {
					immunity_distribution.push_back(immunity_rate);
				}
			} else {
				const std::string xml_immunity_profile =
						"disease.immunity_profile."
								+ StringUtils::ToLower(immunity_profile);
				immunity_distribution = PtreeUtils::GetDistribution(pt_disease,
						xml_immunity_profile);
			}
			Administer(immunity_clusters, immunity_distribution,
					immunity_link_probability, rng);
		}
	}
	std::cout << "Done building: " << s << " profile " << endl;
}

} // end_of_namespace
