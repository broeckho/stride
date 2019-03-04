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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header for the Immunizer class.
 */

#pragma once

#include "pool/ContactPool.h"
#include "pop/Person.h"
#include "util/RnMan.h"

#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>
#include <vector>

namespace stride {

/**
 * Deals with immunization strategies.
 */
class Immunizer
{
public:
        ///
        explicit Immunizer(util::RnMan& rnManager) : m_rn_manager(rnManager) {}

        /// Random immunization.
        template <typename T>
        void Random(T& pools, std::vector<double>& immunityDistribution, double immunityLinkProbability)
        {
        		// Sampler for int in [0, pools.size()) and for double in [0.0, 1.0).
        		const auto poolsSize = static_cast<int>(pools.size());
        		auto intGenerator = m_rn_manager[0].variate_generator(trng::uniform_int_dist(0, poolsSize));
        		auto uniform01Generator = m_rn_manager[0].variate_generator(trng::uniform01_dist<double>());

        		// Initialize a vector to count the population per age class [0-100].
        		std::vector<double> populationBrackets(100, 0.0);

        		// Count individuals per age class
        		for (auto& c : pools) {
        			for (const auto& p : c.GetPool()) {
        					populationBrackets[p->GetAge()]++;
        			}
        		}

        		// Calculate the number of immune individuals per age class.
        		unsigned int numImmune = 0;
        		for (unsigned int age = 0; age < 100; age++) {
        			populationBrackets[age] = floor(populationBrackets[age] * immunityDistribution[age]);
        			numImmune += static_cast<unsigned int>(populationBrackets[age]);
        		}

        		// Immunize susceptible individuals, until all age-dependent quota are reached.
        		while (numImmune > 0) {
        			// Select random contact pool
        			const ContactPool& p_pool = pools[intGenerator()];
        			const auto size = static_cast<unsigned int>(p_pool.GetSize());

        			// Random order of indices
        			std::vector<unsigned int> indices(size);
        			iota(indices.begin(), indices.end(), 0U);
        			m_rn_manager[0].shuffle(indices.begin(), indices.end());

        			// Loop over members in random order
        			for (unsigned int i_p = 0; i_p < size && numImmune > 0; i_p++) {
        				Person& p = *p_pool.GetMember(indices[i_p]);
        				// If p is susceptible and age class has not reached quota => make immune
        				if (p.GetHealth().IsSusceptible() && populationBrackets[p.GetAge()] > 0) {
        					p.GetHealth().SetImmune();
        					populationBrackets[p.GetAge()]--;
        					numImmune--;
        				}
        				// Random draw to continue in this pool or sample a new one
        				if (uniform01Generator() < (1 - immunityLinkProbability) ) {
        					break;
        				}
        			}

        		}
        }

        /// Cocoon immunization.
        template <typename T>
        void Cocoon(const T& /*pools*/, std::vector<double>& /*immunityDistribution*/,
                    double /*immunityLinkProbability*/)
        {
                /*
                 * void Vaccinator::AdministerCocoon(const vector<ContactPool>& pools, double immunity_rate, double
        adult_age_min, double adult_age_max, double child_age_min, double child_age_max)
        {
        // Sampler for double in [0.0, 1.0).
        auto uniform01_generator = m_rn_manager[0].variate_generator(trng::uniform01_dist<double>());
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
        }*/
        }

private:
        util::RnMan& m_rn_manager; ///< Random number manager.
};

} // namespace stride
