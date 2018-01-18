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
 * Header for the Random Number Generator class.
 */

#include <trng/mrg2.hpp>
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>

#include <vector>

namespace stride {
namespace util {

/**
 * The random number generator.
 */
class Random
{
public:
        /// Constructor: initialize the random number engine and distribution.
        Random(const unsigned long seed)
        {
                m_engine.seed(seed);
                m_uniform_dist = trng::uniform01_dist<double>();
        }

        /// Get random double.
        double NextDouble() { return m_uniform_dist(m_engine); }

        /// Get random unsigned int from [0, max[.
        unsigned int operator()(unsigned int max)
        {
                trng::uniform_int_dist dis(0, max);
                return dis(m_engine);
        }

        /**
         * Split random engines
         * E. g. stream 0 1 2 3 4 5...
         * => stream A: 0 2 4...
         * => stream B: 1 3 5...
         */
        void Split(unsigned int total, unsigned int id) { m_engine.split(total, id); }

        std::vector<unsigned int> GetRandomIndices(unsigned int size)
        {

                std::vector<unsigned int> indices;
                for (unsigned int i = 0; i < size; i++) {
                        indices.push_back(i);
                }

                std::random_shuffle(indices.begin(), indices.end(), m_engine);

                return indices;
        }

private:
        trng::mrg2 m_engine;                         ///< The random number engine.
        trng::uniform01_dist<double> m_uniform_dist; ///< The random distribution.
};

} // namespace util
} // namespace stride
