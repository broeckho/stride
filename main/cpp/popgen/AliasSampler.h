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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Alias method header.
 */

#include <deque>
#include <map>
#include <random>
#include <vector>

namespace stride {
namespace util {

using namespace std;

/// Usage is very simple, construct with a vector of probabilities,
/// then use as a distribution from the standard library (i.e. with operator()).
class AliasSampler
{
public:
        /// No default constructor.
        AliasSampler() = delete;

        /// Construct the distribution using the method described here:
        /// http://keithschwarz.com/darts-dice-coins/
        /// @param probabilities		A vector with length n.
        explicit AliasSampler(const vector<double>& probabilities);

        /// Draw from the probability distribution.
        /// @param gen		A random generator providing unsigned in and double01 draws.
        /// @return			A random (weighted) integer in [0, n)
        template <typename RNG_ENGINE>
        unsigned int operator()(RNG_ENGINE& gen)
        {
                const unsigned int i = m_diceroll(gen);
                return m_coinflip(gen) < m_blocks[i].prob ? i : m_blocks[i].alias;
        }

private:
        /// Structure used in the implementation.
        struct AliasBlock
        {
                double       prob;
                unsigned int alias;
        };

private:
        std::vector<AliasBlock>                     m_blocks;
        std::uniform_int_distribution<unsigned int> m_diceroll;
        std::uniform_real_distribution<double>      m_coinflip;
};

} // namespace util
} // namespace stride
