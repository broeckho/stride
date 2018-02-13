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

#include "AliasMethod.h"

namespace stride {
namespace generator {

using namespace std;

template <typename T>
AliasMethod<T>::AliasMethod(unsigned long seed) : m_seed(seed)
{
        m_rng = util::RNG<trng::mrg2>(m_seed);
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
        m_value_map.erase(static_cast<unsigned int>(m_original_table.size()));
        m_original_table.erase(m_original_table.begin() + index);
        m_was_built = false;
}

template <typename T>
void AliasMethod<T>::BuildSampler()
{
        // see http://keithschwarz.com/darts-dice-coins/

        m_was_built = true;

        // Step 1: Alias and Prob arrays of size n.
        const auto table_size = m_original_table.size();
        m_prob_table.resize(table_size);
        if (m_original_table.size() == 1) { // There's only one thing to sample from.
                m_prob_table[0] = 1;
                return;
        }
        m_alias_table.resize(table_size);

        std::vector<bool> has_alias_table(table_size, false);

        // Normalize probabilities and multiply by n.
        double prob_sum = 0;
        for (unsigned int i = 0; i < table_size; ++i) {
                prob_sum += m_original_table[i];
        }
        for (unsigned int i = 0; i < table_size; ++i) {
                m_prob_table[i] = m_original_table[i] * table_size / prob_sum;
        }

        // Find appropriate probabilities (& fix rounding errors in single pass)
        bool         found_low  = true;
        bool         found_high = true;
        unsigned int low_index  = 0;
        unsigned int high_index = 0;
        while (found_low && found_high) {
                // Single pass find something beneath & find something above.
                found_low        = false;
                found_high       = false;
                const double eps = std::numeric_limits<double>::epsilon();
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
                                m_alias_table[low_index]   = high_index;
                                m_prob_table[high_index] -= (1.0l - m_prob_table[low_index]);
                                break;
                        }
                }
        }
}

template <typename T>
unsigned int AliasMethod<T>::SampleIndex()
{
        const unsigned int index = RandomIndex();
        return Random01() < m_prob_table[index] ? index : m_alias_table[index];
}

template <typename T>
const T& AliasMethod<T>::Sample()
{
        return m_value_map[SampleIndex()];
}

template <typename T>
T AliasMethod<T>::SampleValue()
{
        return m_value_map[SampleIndex()];
}

template <typename T>
tuple<unsigned int, T> AliasMethod<T>::SampleIndexValue()
{
        unsigned int index = SampleIndex();
        T            value = m_value_map[index];
        return std::make_tuple(index, value);
}

template class AliasMethod<std::vector<unsigned int>>;
template class AliasMethod<size_t>;

} // namespace generator
} // namespace stride
