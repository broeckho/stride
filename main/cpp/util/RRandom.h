#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED
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
 *  Copyright 2017, Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Header for the Random Number Generator class.
 */

// Engines
#include <trng/lagfib2plus.hpp>
#include <trng/lagfib2xor.hpp>
#include <trng/lagfib4plus.hpp>
#include <trng/lagfib4xor.hpp>
#include <trng/lcg64.hpp>
#include <trng/lcg64_shift.hpp>
#include <trng/mrg2.hpp>
#include <trng/mrg3.hpp>
#include <trng/mrg3s.hpp>
#include <trng/mrg4.hpp>
#include <trng/mrg5.hpp>
#include <trng/mrg5s.hpp>
#include <trng/yarn2.hpp>
#include <trng/yarn3.hpp>
#include <trng/yarn3s.hpp>
#include <trng/yarn4.hpp>
#include <trng/yarn5.hpp>
#include <trng/yarn5s.hpp>

// Distributions
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>

namespace stride {
namespace util {

class RNGInterface
{
public:
        virtual void Seed(unsigned long seed) = 0;
        virtual unsigned int operator()(unsigned int max) = 0;
};

/**
 * The random number generator.
 */
template <typename EngineType>
class Random : public RNGInterface
{
public:
        /// Constructor: initialize the random number engine and distribution.
        Random(const unsigned long seed)
        {
                m_engine.seed(seed);
                m_uniform_dist = trng::uniform01_dist<double>();
        }

        /// Default Constructor
        Random()
        {
                // Needed to avoid ambiguous call to seed() for engines lcg64 and lcg64_shift
                unsigned long s = 0;

                m_engine.seed(s);
                m_uniform_dist = trng::uniform01_dist<double>();
        }

        Random(EngineType& engine)
        {
                m_engine = engine;
                m_uniform_dist = trng::uniform01_dist<double>();
        }

        void Seed(const unsigned long seed) { m_engine.seed(seed); }

        /// Get random double.
        double NextDouble() { return m_uniform_dist(m_engine); }

        /// Get random unsigned int from [0, max[.
        virtual unsigned int operator()(unsigned int max)
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

        const EngineType& GetEngine() const { return m_engine; }

private:
        EngineType m_engine;                         ///< The random number engine.
        trng::uniform01_dist<double> m_uniform_dist; ///< The random distribution.
};

} // namespace util
} // namespace stride

#endif // include guard
