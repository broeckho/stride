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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Interface of RNManager.
 */

#include "RNEngineType.h"

#include <trng/lcg64.hpp>
#include <trng/lcg64_shift.hpp>
#include <trng/mrg2.hpp>
#include <trng/mrg3.hpp>
#include <trng/yarn2.hpp>
#include <trng/yarn3.hpp>
#include <functional>
#include <string>
#include <vector>

namespace stride {
namespace util {

/**
 * Manages random number generation in parallel (OpenMP) calculations.
 * Internally, TRNG engines are used.
 */
class RNManager
{
public:
        /// POD representation of the RNManager's state. If no state is available,
        /// i.e. state is an empty string, the initial state corresponding to the
        /// seed is implied. If a state is available, the seed is disregarded.
        struct Info
        {
                explicit Info(std::string type = "mrg2", unsigned long seed = 1UL, std::string state = "",
                              unsigned int stream_count = 1U)
                    : m_seed(seed), m_state(std::move(state)), m_stream_count(stream_count), m_type(std::move(type)){};

                unsigned long m_seed;         ///< Seed for the engine.
                std::string   m_state;        ///< Long string reperesting current state.
                unsigned int  m_stream_count; ///< Number of streams set up with the engine.
                std::string   m_type;         ///< Type id (see RNEngineType).
        };

public:
        /// Initializes to default engine( trng::mrg2 with seed 1).
        explicit RNManager(const Info& info = Info());

        /// No copying.
        RNManager(const RNManager&) = delete;

        /// No copy assignement.
        RNManager& operator=(const RNManager&) = delete;

        /// Return the state of the random engine.
        Info GetInfo() const;

        /// Produce a random generator out of a distribution bound to sim's random engine.
        /// @param   d   The distribution object.
        /// @param   i   Which stream gets sampled, 0 by default.
        /// @return      The random generator g that can be called as g()
        template <typename D>
        std::function<typename D::result_type()> GetGenerator(const D& d, size_t i = 0)
        {
                using namespace RNEngineType;
                std::function<typename D::result_type()> gen;
                switch (m_type_id) {
                case Id::lcg64: gen = std::bind(d, std::ref(m_lcg64[i])); break;
                case Id::lcg64_shift: gen = std::bind(d, std::ref(m_lcg64_shift[i])); break;
                case Id::mrg2: gen = std::bind(d, std::ref(m_mrg2[i])); break;
                case Id::mrg3: gen = std::bind(d, std::ref(m_mrg3[i])); break;
                case Id::yarn2: gen = std::bind(d, std::ref(m_yarn2[i])); break;
                case Id::yarn3: gen = std::bind(d, std::ref(m_yarn3[i])); break;
                }
                return gen;
        }

        /// Randomly shuffle items between iterators using i-th stream of the random engine.
        /// \param first
        /// \param last
        /// \return
        template <typename RandomIt>
        void RandomShuffle(RandomIt first, RandomIt last, size_t i = 0)
        {
                using namespace RNEngineType;
                switch (m_type_id) {
                case Id::lcg64: std::random_shuffle(first, last, m_lcg64[i]); break;
                case Id::lcg64_shift: std::random_shuffle(first, last, m_lcg64_shift[i]); break;
                case Id::mrg2: std::random_shuffle(first, last, m_mrg2[i]); break;
                case Id::mrg3: std::random_shuffle(first, last, m_mrg3[i]); break;
                case Id::yarn2: std::random_shuffle(first, last, m_yarn2[i]); break;
                case Id::yarn3: std::random_shuffle(first, last, m_yarn3[i]); break;
                }
        }

        /// Initalize with data in Info.
        void Initialize(const Info& info = Info());

private:
        unsigned long    m_seed;         ///< Actual seed used with random engine.
        unsigned int     m_stream_count; ///< Number of streams set up with the engine.
        RNEngineType::Id m_type_id;      ///< Identifies type of random engine.

private:
        std::vector<trng::lcg64>       m_lcg64;
        std::vector<trng::lcg64_shift> m_lcg64_shift;
        std::vector<trng::mrg2>        m_mrg2;
        std::vector<trng::mrg3>        m_mrg3;
        std::vector<trng::yarn2>       m_yarn2;
        std::vector<trng::yarn3>       m_yarn3;
};

} // namespace util
} // namespace stride
