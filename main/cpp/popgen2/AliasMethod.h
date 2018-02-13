#pragma once
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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the AliasMethod class template.
 */

#include "RNG.h"

#include <unordered_map>
#include <vector>

namespace stride {
namespace generator {

/// The AliasMethod is an efficient technique for sampling from a discrete set
/// outcomes 0, 1, ... , n-1 (labels for whatver the oucomes are) with associated
/// probability distribution p_0, p_1,  ... , p_n-1.
template <typename T>
class AliasMethod
{
public:
        /// Initializing constructor.
        explicit AliasMethod(unsigned long seed = 0);

        /// Destructor
        virtual ~AliasMethod() = default;

        /// Add an outcome and its probability to the discrete set.
        /// The probability distribution will be rescaled ( to sum_of_p_i equals 1).
        void Add(T value, double probability);

        /// Remove an outome from the the discrete set. This requires you to rebuild
        // the sampler after all removals have taken place.
        void Remove(unsigned int index);

        /// Builds the sampler tables. Needed prior to using any of the sampling methods.
        void BuildSampler();

        /// Samples an index from the AliasMethod. Indices are assigned in order of addition to the table.
        unsigned int SampleIndex();

        /// Sample and return a const reference to the sampled item.
        const T& Sample();

        /// Samples from the AliasMethod and returns a copy of the sampled item.
        T SampleValue();

        ///
        std::tuple<unsigned int, T> SampleIndexValue();

private:
public:
        /// Random unsigned int in [0, n)
        unsigned int RandomIndex() { return m_rng(static_cast<unsigned int>(m_original_table.size())); }

        /// Random double in [0, 1]
        double Random01() { return m_rng.NextDouble01(); }

private:
        unsigned long                       m_seed;              ///< RNG seed.
        util::RNG<trng::mrg2>               m_rng;               ///< RNG.
        bool                                m_was_built = false; ///< Tracks table built status.
        std::unordered_map<unsigned int, T> m_value_map;         ///< Maps indices to items.
        std::vector<double>                 m_original_table;    ///< Table things get added to.
        std::vector<double>                 m_prob_table;        ///< Probability table (after being built)
        std::vector<unsigned int>           m_alias_table;       ///< Alias table (after being built)
};

extern template class AliasMethod<std::vector<unsigned int>>;
extern template class AliasMethod<size_t>;

} // namespace generator
} // namespace stride
