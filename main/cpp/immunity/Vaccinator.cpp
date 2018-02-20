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

//#include "util/PtreeUtils.h"
#include "util/StringUtils.h"
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>

#include <iostream>

namespace stride {

using namespace util;

/// Primary immunization profile: do nothing
template <ImmunizationProfile profile>
class Immunizer
{
public:
        static void Administer(const std::vector<ContactPool>& clusters, std::vector<double>& immunity_distribution,
                               double immunity_link_probability, util::RNManager& rn_manager)
        {
        }
};

/// Random immunization profile
template <>
class Immunizer<ImmunizationProfile::Random>
{
public:
        static void Administer(const std::vector<ContactPool>& clusters, std::vector<double>& immunity_distribution,
                               double immunity_link_probability, util::RNManager& rn_manager)
        {
                std::cout << "Applying random immunity profile" << std::endl;
                // Initialize a vector to count the population per age class [0-100].
                std::vector<double> population_count_age(100, 0.0);

                // Count individuals per age class and set all "susceptible" individuals "immune".
                // note: focusing on measles, we expect the number of susceptible individuals
                // to be less compared to the number of immune.
                // TODO but this is a generic simulator
                for (auto& c : clusters) {
                        for (unsigned int i_p = 0; i_p < c.GetSize(); i_p++) {
                                Person& p = *c.GetMember(i_p);
                                if (p.GetHealth().IsSusceptible()) {
                                        p.GetHealth().SetImmune();
                                        population_count_age[p.GetAge()]++;
                                }
                        }
                }

                // Sampler for int in [0, clusters.size())
                const auto clusters_size = static_cast<int>(clusters.size());
                auto       int_generator = rn_manager.GetGenerator(trng::uniform_int_dist(0, clusters_size));
                // Sampler for double in [0.0, 1.0).
                auto uniform01_generator = rn_manager.GetGenerator(trng::uniform01_dist<double>());

                // Calculate the number of susceptible individuals per age class.
                unsigned int total_num_susceptible = 0;
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        population_count_age[index_age] =
                            floor(population_count_age[index_age] * (1 - immunity_distribution[index_age]));
                        total_num_susceptible += population_count_age[index_age];
                }

                // Sample susceptible individuals, until all age-dependent quota are reached.
                while (total_num_susceptible > 0) {
                        // random cluster, random order of members
                        const ContactPool&        p_cluster = clusters[int_generator()];
                        const auto                size      = p_cluster.GetSize();
                        std::vector<unsigned int> indices(size);
                        for (size_t i = 0; i < size; i++) {
                                indices[i] = static_cast<unsigned int>(i); // TODO why not just loop over unsigned ints?
                        }
                        rn_manager.RandomShuffle(indices.begin(), indices.end());

                        // loop over cluster members, in random order
                        for (unsigned int i_p = 0; i_p < size && total_num_susceptible > 0; i_p++) {
                                Person& p = *p_cluster.GetMember(indices[i_p]);
                                // if p is immune and his/her age class has not reached the quota => make susceptible
                                if (p.GetHealth().IsImmune() && population_count_age[p.GetAge()] > 0) {
                                        p.GetHealth().SetSusceptible();
                                        population_count_age[p.GetAge()]--;
                                        total_num_susceptible--;
                                }
                                // random draw to continue in this cluster or to sample a new one
                                if (uniform01_generator() < (1 - immunity_link_probability)) {
                                        break;
                                }
                        }
                }
        }
};

/// Profile for cocoon vaccination strategy
template <>
class Immunizer<ImmunizationProfile::Cocoon>
{
public:
        static void Administer(const std::vector<ContactPool>& clusters, std::vector<double>& immunity_distribution,
                               double immunity_link_probability, util::RNManager& rn_manager)
        {
                std::cout << "Applying cocoon immunity profile" << std::endl;
                /*
                 * void Vaccinator::AdministerCocoon(const vector<ContactPool>& clusters, double immunity_rate, double
adult_age_min, double adult_age_max, double child_age_min, double child_age_max)
{
        // Sampler for double in [0.0, 1.0).
        auto uniform01_generator = m_rn_manager.GetGenerator(trng::uniform01_dist<double>());
        for (const auto& c : clusters) {
                for (unsigned int i_p = 0; i_p < c.GetSize(); i_p++) {
                        Person& p = *c.GetMember(i_p);
                        if (p.GetHealth().IsSusceptible() && p.GetAge() >= adult_age_min &&
                            p.GetAge() <= adult_age_max) {

                                bool is_connected_to_target_age{false};
                                for (unsigned int i_p2 = 0; i_p2 < c.GetSize() && !is_connected_to_target_age; i_p2++) {
                                        const Person& p2 = *c.GetMember(i_p2);
                                        if (p2.GetAge() >= child_age_min && p2.GetAge() <= child_age_max) {
                                                is_connected_to_target_age = true;
                                        }
                                }
                                if (is_connected_to_target_age && uniform01_generator() < immunity_rate) {
                                        p.GetHealth().SetImmune();
                                }
                        }
                }
        }
}
                 *
                 */
        }
};

Vaccinator::Vaccinator(const boost::property_tree::ptree& pt_config, util::RNManager& rn_manager)
    : m_pt_config(pt_config), m_rn_manager(rn_manager)
{
}

void Vaccinator::Apply(std::shared_ptr<Simulator> sim)
{
        // Get immunity and vaccination profiles
        std::string immunity_profile    = m_pt_config.get<std::string>("run.immunity_profile");
        std::string vaccination_profile = m_pt_config.get<std::string>("run.vaccine_profile");

        // Apply natural immunity in the population
        std::cout << "Applying natural immunity to population ..." << std::endl;
        Administer("immunity", immunity_profile, sim);

        // Apply vaccination in the population
        std::cout << "Applying vaccination strategy to population ..." << std::endl;
        Administer("vaccine", vaccination_profile, sim);
}

void Vaccinator::Administer(std::string immunity_type, std::string immunization_profile, std::shared_ptr<Simulator> sim)
{
        std::vector<double> immunity_distribution;
        const double        immunity_link_probability = 0;
        // const double immunity_link_probability = ((immunization_profile == "Cocoon") ? 1 :
        // m_pt_config.get<double>("run." + ToLower(immunity_type) + "_link_probability"));
        std::vector<ContactPool>* immunity_clusters = &sim->m_households; ///< The default case.

        /*
         *                 if (immunity_link_probability > 0) {
                        using namespace ContactPoolType;
                        Id c_type = ToType(m_config.get<string>("run." + ToLower(s) + "_link_clustertype"));
                        switch (c_type) {
                        case Id::Household: immunity_clusters = &m_sim->m_households; break;
                        case Id::School: immunity_clusters = &m_sim->m_school_clusters; break;
                        case Id::Work: immunity_clusters = &m_sim->m_work_clusters; break;
                        case Id::PrimaryCommunity: immunity_clusters = &m_sim->m_primary_community; break;
                        case Id::SecondaryCommunity: immunity_clusters = &m_sim->m_secondary_community; break;
                        }
                }
         */
        if (immunization_profile == "Random") {
                const auto immunity_rate = m_pt_config.get<double>("run." + ToLower(immunity_type) + "_rate");
                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                        immunity_distribution.push_back(immunity_rate);
                }
                Immunizer<ImmunizationProfile::Random>::Administer(*immunity_clusters, immunity_distribution,
                                                                   immunity_link_probability, m_rn_manager);
        } else if (immunization_profile == "Cocoon") {
                Immunizer<ImmunizationProfile::Cocoon>::Administer(*immunity_clusters, immunity_distribution,
                                                                   immunity_link_probability, m_rn_manager);
        } else {
                Immunizer<ImmunizationProfile::None>::Administer(*immunity_clusters, immunity_distribution,
                                                                 immunity_link_probability, m_rn_manager);
        }
}

} // namespace stride
