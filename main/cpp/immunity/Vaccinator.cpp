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
using boost::property_tree::ptree;

Vaccinator::Vaccinator(std::shared_ptr<Simulator> sim, const boost::property_tree::ptree& pt_config,
                       const boost::property_tree::ptree& pt_disease, util::Random& rng)
    : m_config(pt_config), m_disease(pt_disease), m_sim(sim), m_rng(rng)
{
}

void Vaccinator::Administer(const vector<Cluster>& clusters, vector<double>& immunity_distribution,
                            double immunity_link_probability)
{
        // Initialize a vector to count the population per age class [0-100].
        vector<double> population_count_age(100, 0.0);

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

        // Calculate the number of susceptible individuals per age class.
        unsigned int total_num_susceptible = 0;
        for (unsigned int index_age = 0; index_age < 100; index_age++) {
                population_count_age[index_age] =
                    floor(population_count_age[index_age] * (1 - immunity_distribution[index_age]));
                total_num_susceptible += population_count_age[index_age];
        }

        // Sample susceptible individuals, until all age-dependent quota are reached.
        while (total_num_susceptible > 0) {
                // sample cluster
                const Cluster& p_cluster = clusters[static_cast<unsigned int>(m_rng(clusters.size()))];
                const auto size = static_cast<unsigned int>(p_cluster.GetSize());
                vector<unsigned int> indices = m_rng.GetRandomIndices(size);

                // loop over cluster members, in random order
                for (unsigned int i_p = 0; i_p < size && total_num_susceptible > 0; i_p++) {
                        Person& p = *p_cluster.GetMember(indices[i_p]);

                        // if individual is immune and his/her age class has not reached the quota
                        // => make
                        // susceptible
                        if (p.GetHealth().IsImmune() && population_count_age[p.GetAge()] > 0) {
                                p.GetHealth().SetSusceptible();
                                population_count_age[p.GetAge()]--;
                                total_num_susceptible--;
                        }
                        // random draw to continue in this cluster or to sample a new one
                        if (m_rng.NextDouble() < (1 - immunity_link_probability)) {
                                break;
                        }
                }
        }
}

void Vaccinator::AdministerCocoon(const vector<Cluster>& clusters, double immunity_rate, double adult_age_min,
                                  double adult_age_max, double child_age_min, double child_age_max)
{
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
                                if (is_connected_to_target_age && m_rng.NextDouble() < immunity_rate) {
                                        p.GetHealth().SetImmune();
                                }
                        }
                }
        }
}

void Vaccinator::Apply(const string& s)
{
        const string profile = m_config.get<string>("run." + ToLower(s) + "_profile");
        cout << "Building: " << s << " profile => " << profile << endl;

        if (profile != "None") {

                vector<double> immunity_distribution;
                const double immunity_link_probability =
                    ((profile == "Cocoon") ? 1 : m_config.get<double>("run." + ToLower(s) + "_link_probability"));

                vector<Cluster>* immunity_clusters = &m_sim->m_households; ///< The default case.
                if (immunity_link_probability > 0) {
                        using namespace ClusterType;
                        Id c_type = ToType(m_config.get<string>("run." + ToLower(s) + "_link_clustertype"));
                        switch (c_type) {
                        case Id::Household: immunity_clusters = &m_sim->m_households; break;
                        case Id::School: immunity_clusters = &m_sim->m_school_clusters; break;
                        case Id::Work: immunity_clusters = &m_sim->m_work_clusters; break;
                        case Id::PrimaryCommunity: immunity_clusters = &m_sim->m_primary_community; break;
                        case Id::SecondaryCommunity: immunity_clusters = &m_sim->m_secondary_community; break;
                        default: throw runtime_error(string(__func__) + "Link cluster type screwed up!");
                        }
                }
                if (profile == "Cocoon") {
                        cout << "cocoon" << endl;
                        const auto immunity_rate = m_config.get<double>("run." + ToLower(s) + "_rate");
                        const auto adult_age_min = m_config.get<double>("run." + ToLower(s) + "_adult_age_min");
                        const auto adult_age_max = m_config.get<double>("run." + ToLower(s) + "_adult_age_max");
                        const auto child_age_min = m_config.get<double>("run." + ToLower(s) + "_child_age_max");
                        const auto child_age_max = m_config.get<double>("run." + ToLower(s) + "_child_age_max");

                        AdministerCocoon(*immunity_clusters, immunity_rate, adult_age_min, adult_age_max, child_age_min,
                                         child_age_max);
                } else {
                        if (profile == "Random") {
                                const auto immunity_rate = m_config.get<double>("run." + ToLower(s) + "_rate");
                                for (unsigned int index_age = 0; index_age < 100; index_age++) {
                                        immunity_distribution.push_back(immunity_rate);
                                }
                        } else {
                                const string xml_immunity_profile = "disease.immunity_profile." + ToLower(profile);
                                immunity_distribution = PtreeUtils::GetDistribution(m_disease, xml_immunity_profile);
                        }
                        Administer(*immunity_clusters, immunity_distribution, immunity_link_probability);
                }
        }
        cout << "Done building: " << s << " profile " << endl;
}

} // namespace stride
