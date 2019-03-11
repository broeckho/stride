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
 * Interface of RnPcg.
 */

#pragma once

#include "StringUtils.h"

//#include <trng/lcg64.hpp>
#include <iostream>
#include <pcg/pcg_random.hpp>
#include <randutils/randutils.hpp>
#include <string>
#include <vector>

namespace stride {
namespace util {

/**
 * Manages random number generation in parallel (OpenMP) calculations.
 */
template <typename E>
class Rn : protected std::vector<randutils::random_generator<E, randutils::seed_seq_fe128>>
{
public:
        using EngineType    = E;
        using RnType        = randutils::random_generator<E, randutils::seed_seq_fe128>;
        using ContainerType = std::vector<randutils::random_generator<E, randutils::seed_seq_fe128>>;

public:
        /// POD representation of the RNManager's state. If no state is available, i.e. state
        /// is an empty string, the initial state corresponding to the seed sequence is implied.
        /// If a state is available, the seed sequence is disregarded.
        struct Info
        {
                explicit Info(std::string seed_seq_init = "1,2,3,4", std::string state = "",
                              unsigned int stream_count = 1U)
                    : m_seed_seq_init(std::move(seed_seq_init)), m_state(std::move(state)),
                      m_stream_count(stream_count){};

                std::string  m_seed_seq_init; ///< Seed for the engine.
                std::string  m_state;         ///< Long string representing current state.
                unsigned int m_stream_count;  ///< Number of streams set up with the engine.
        };

public:
        using ContainerType::operator[];
        using ContainerType::at;
        using ContainerType::size;

        /// Default constructor build empty manager.
        Rn() : ContainerType(), m_seed_seq_init(""), m_stream_count(0U) {}

        /// Initializes.
        explicit Rn(const Info& info)
            : ContainerType(info.m_stream_count), m_seed_seq_init(info.m_seed_seq_init),
              m_stream_count(info.m_stream_count)
        {
                Initialize(info);
        }

        /// No copying.
        Rn(const Rn&) = delete;

        /// No copy assignment.
        Rn& operator=(const Rn&) = delete;

        /// Equality of states
        bool operator==(const Rn& other);

        /// Return the state of the random engines.
        Info GetInfo() const;

        /// Initalize with data in Info.
        void Initialize(const Info& info);

        /// Is this een empty (i.e. non-initialized Rn)?
        bool IsEmpty() const { return ContainerType::empty() || (m_stream_count == 0U); }

private:
        /// Actual first-time seeding. Procedure varies according to engine type, see specialisations.
        void Seed(randutils::seed_seq_fe128& seseq);

private:
        std::string  m_seed_seq_init; ///< Seed sequence initializer used with engines.
        unsigned int m_stream_count;  ///< Number of threads/streams set up with the engine.
};

template <>
void Rn<pcg64>::Seed(randutils::seed_seq_fe128& seseq);

extern template class Rn<pcg64>;
// extern template class Rn<trng::lcg64>;

// template<typename E>
inline std::ostream& operator<<(std::ostream& os, const typename Rn<pcg64>::Info& info)
{
        os << "Seed sequence: " << info.m_seed_seq_init << "\n"
           << "Number of streams: " << info.m_stream_count << "\n"
           << "State: " << info.m_state;
        return os;
}

} // namespace util
} // namespace stride
