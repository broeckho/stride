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
 * Implementation for the Immunizer class.
 */

#include "Immunizer.h"

#include "pop/Person.h"
#include "util/RNManager.h"

#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>

namespace stride {

using namespace util;

Immunizer::Immunizer(stride::util::RNManager& rn_manager) : m_rn_manager(rn_manager) {}

void Immunizer::Random(const std::vector<ContactPool>& pools, std::vector<double>& immunity_distribution,
                       double immunity_link_probability)
{
        // Initialize a vector to count the population per age class [0-100].
        std::vector<double> population_count_age(100, 0.0);

        // Count individuals per age class and set all "susceptible" individuals "immune".
        // note: focusing on measles, we expect the number of susceptible individuals
        // to be less compared to the number of immune.
        // TODO but this is a generic simulator
        for (auto& c : pools) {
                for (const auto& p : c.GetPool()) {
                        if (p->GetHealth().IsSusceptible()) {
                                p->GetHealth().SetImmune();
                                population_count_age[p->GetAge()]++;
                        }
                }
        }

        // Sampler for int in [0, pools.size()) and for double in [0.0, 1.0).
        const auto pools_size          = static_cast<int>(pools.size());
        auto       int_generator       = m_rn_manager.GetGenerator(trng::uniform_int_dist(0, pools_size));
        auto       uniform01_generator = m_rn_manager.GetGenerator(trng::uniform01_dist<double>());

        // Calculate the number of susceptible individuals per age class.
        unsigned int total_num_susceptible = 0;
        for (unsigned int index_age = 0; index_age < 100; index_age++) {
                population_count_age[index_age] =
                    floor(population_count_age[index_age] * (1 - immunity_distribution[index_age]));
                total_num_susceptible += population_count_age[index_age];
        }

        // Sample susceptible individuals, until all age-dependent quota are reached.
        while (total_num_susceptible > 0) {
                // random pool, random order of members
                const ContactPool&        p_pool = pools[int_generator()];
                const auto                size   = static_cast<unsigned int>(p_pool.GetSize());
                std::vector<unsigned int> indices(size);
                for (unsigned int i = 0; i < size; i++) {
                        indices[i] = i;
                }
                m_rn_manager.RandomShuffle(indices.begin(), indices.end());

                // loop over members, in random order
                for (unsigned int i_p = 0; i_p < size && total_num_susceptible > 0; i_p++) {
                        Person& p = *p_pool.GetMember(indices[i_p]);
                        // if p is immune and his/her age class has not reached the quota => make susceptible
                        if (p.GetHealth().IsImmune() && population_count_age[p.GetAge()] > 0) {
                                p.GetHealth().SetSusceptible();
                                population_count_age[p.GetAge()]--;
                                total_num_susceptible--;
                        }
                        // random draw to continue in this pool or to sample a new one
                        if (uniform01_generator() < (1 - immunity_link_probability)) {
                                break;
                        }
                }
        }
}

void Immunizer::Cocoon(const std::vector<ContactPool>& /*pools*/, std::vector<double>& /*immunity_distribution*/,
                       double /*immunity_link_probability*/)
{
        /*
         * void Vaccinator::AdministerCocoon(const vector<ContactPool>& pools, double immunity_rate, double
adult_age_min, double adult_age_max, double child_age_min, double child_age_max)
{
// Sampler for double in [0.0, 1.0).
auto uniform01_generator = m_rn_manager.GetGenerator(trng::uniform01_dist<double>());
for (const auto& c : pools) {
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

} // namespace stride
