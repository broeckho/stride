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

#include "util/RRandom.h"

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace stride {
namespace generator {

template <typename T>
class AliasMethod
{
public:
        /// Initializing constructor.
        AliasMethod(unsigned long seed = 0);

        /// Destructor
        virtual ~AliasMethod() = default;

        /// Seeds the random number generator used with the alias method.
        void Seed(unsigned long seed);

        /// Adds an item the alias method can choose from, with a probability. The AliasMethod class will
        /// automatically scale any probabilities to the [0, 1] range when the sampler is built.
        void Add(T value, double probability);

        ///
        void Remove(unsigned int index);

        /// Builds the sample tables. This method rescales the probabilities to the [0, 1] range and builds the
        /// probability and alias vectors.
        void BuildSampler();

        /// Returns whether the tables have been built, and the AliasMethod is ready to be sampled.
        bool WasBuilt();

        /// Returns the size of the (non-built) alias table.
        unsigned int GetSize();

        /// Samples an index from the AliasMethod. Indices are assigned in order of addition to the table.
        /// If for example one would add b and then a to the alias table, b would have index 0, and a would have
        /// index 1.
        unsigned int SampleIndex();

        /// Samples from the AliasMethod and returns a const reference to the sampled item.
        const T& SampleCref();

        /// Samples from the AliasMethod and returns a copy of the sampled item.
        T SampleValue();

        ///
        std::tuple<unsigned int, T> SampleIndexValue();

        // TODO: Move this.
        /// Utility value.
        const static double RAND_MAX_double;

private:
        /// Random int in [0, n)
        inline unsigned int RandomIndex() { return m_rng(m_original_table.size()); }

        /// Random double in [0, 1]
        inline double RandomFraction() { return m_rng.NextDouble() / RAND_MAX_double; }

private:
        unsigned long m_seed;                            ///< RNG seed.
        util::Random<trng::mrg2> m_rng;                  ///< RNG.
        bool m_was_built = false;                        ///< Tracks table built status.
        std::unordered_map<unsigned int, T> m_value_map; ///< Maps indices to items.
        std::vector<double> m_original_table;            ///< Table things get added to.
        std::vector<double> m_prob_table;                ///< Probability table (after being built)
        std::vector<unsigned int> m_alias_table;         ///< Alias table (after being built)
};

template <typename T>
const double AliasMethod<T>::RAND_MAX_double = static_cast<double>(RAND_MAX);

//---------------------------------------------
//
//---------------------------------------------

template <typename T>
AliasMethod<T>::AliasMethod(unsigned long seed) : m_seed(seed)
{
        m_rng = util::Random<trng::mrg2>(m_seed);
}

template <typename T>
void AliasMethod<T>::Seed(unsigned long seed)
{
        m_seed = seed;
        m_rng = util::Random<trng::mrg2>(m_seed);
}

template <typename T>
void AliasMethod<T>::Add(T value, double probability)
{
        m_value_map[m_original_table.size()] = value;
        m_original_table.push_back(probability);
        m_was_built = false;
}

template <typename T>
void AliasMethod<T>::Remove(unsigned int index)
{
        for (unsigned int i = index; i < m_original_table.size() - 1; i++) {
                m_value_map[i] = m_value_map[i + 1];
        }
        m_value_map.erase(m_original_table.size());
        m_original_table.erase(m_original_table.begin() + index);
        m_was_built = false;
}

template <typename T>
void AliasMethod<T>::BuildSampler()
{
        m_was_built = true;

        unsigned int table_size = m_original_table.size();

        m_prob_table.resize(table_size);

        if (m_original_table.size() == 1) { // There's only one thing to sample from.
                m_prob_table[0] = 1;
                return;
        }

        m_alias_table.resize(table_size);
        std::vector<bool> has_alias_table(table_size, false);

        // Normalize the table.
        double prob_sum = 0;
        for (unsigned int i = 0; i < table_size; ++i) {
                prob_sum += m_original_table[i];
        }
        for (unsigned int i = 0; i < table_size; ++i) {
                m_prob_table[i] = m_original_table[i] * table_size / prob_sum;
        }

        // Find appropriate probabilities (& fix rounding errors in single pass)
        bool found_low = true, found_high = true;

        unsigned int low_index = -1;
        unsigned int high_index = 0;

        while (found_low && found_high) {
                // Single pass find something beneath & find something above.
                found_low = false;
                found_high = false;
                double eps = std::numeric_limits<double>::epsilon();
                for (unsigned int i = 0; i < table_size; ++i) {
                        if (m_prob_table[i] < 1 - eps && !has_alias_table[i]) {
                                found_low = true;
                                low_index = i;
                        } else if (m_prob_table[i] > 1 + eps) {
                                found_high = true;
                                high_index = i;
                        }

                        if (found_low && found_high) {
                                // Switch things up
                                has_alias_table[low_index] = true;
                                m_alias_table[low_index] = high_index;
                                m_prob_table[high_index] -= (1.0l - m_prob_table[low_index]);
                                break;
                        }
                }
        }
}

template <typename T>
unsigned int AliasMethod<T>::GetSize()
{
        return m_original_table.size();
}

template <typename T>
unsigned int AliasMethod<T>::SampleIndex()
{
        assert(m_was_built);
        unsigned int index = RandomIndex();
        return RandomFraction() < m_prob_table[index] ? index : m_alias_table[index];
}

template <typename T>
const T& AliasMethod<T>::SampleCref()
{
        return m_value_map[SampleIndex()];
}

template <typename T>
T AliasMethod<T>::SampleValue()
{
        return m_value_map[SampleIndex()];
}

template <typename T>
std::tuple<unsigned int, T> AliasMethod<T>::SampleIndexValue()
{
        unsigned int index = SampleIndex();
        T value = m_value_map[index];
        return std::make_tuple(index, value);
}

} // namespace generator
} // namespace stride
