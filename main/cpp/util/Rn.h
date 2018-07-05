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
 * Interface of RnPcg.
 */

#include "StringUtils.h"

#include <trng/lcg64.hpp>
#include <cctype>
#include <functional>
#include <pcg/pcg_random.hpp>
#include <randutils/randutils.hpp>
#include <sstream>
#include <stdexcept>
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

        /// Initializes.
        explicit Rn(const Info& info = Info())
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
        bool operator==(const Rn& other)
        {
                bool status = m_stream_count == other.m_stream_count;
                if (status) {
                        for (size_t i = 0; i < size(); ++i) {
                                status = status && ((*this)[i] == other[i]);
                        }
                }
                return status;
        }

        /// Return the state of the random engines.
        Info GetInfo() const
        {
                Info              info;
                std::stringstream ss;
                for (auto& e : *this) {
                        ss << e.engine();
                }
                info.m_seed_seq_init = m_seed_seq_init;
                info.m_state         = ss.str();
                info.m_stream_count  = m_stream_count;
                return info;
        }

        /// Initalize with data in Info.
        void Initialize(const Info& info = Info())
        {
                if (m_stream_count != info.m_stream_count) {
                        m_stream_count = info.m_stream_count;
                        this->resize(m_stream_count);
                }
                m_seed_seq_init = info.m_seed_seq_init;

                auto state = info.m_state;
                if (state.empty()) {
                        std::vector<unsigned int> seseq_init_vec;
                        for (const auto& e : Split(m_seed_seq_init, ",")) {
                                if (!CheckAllDigits(e)) {
                                        throw std::runtime_error("Rn::Seed> Error in seeding definiton.");
                                }
                                seseq_init_vec.push_back(FromString<unsigned int>(e));
                        }
                        randutils::seed_seq_fe128 seseq(seseq_init_vec.begin(), seseq_init_vec.end());

                        Seed(seseq);
                } else {
                        std::stringstream ss(state);
                        for (size_t i = 0; i < m_stream_count; ++i) {
                                ss >> (*this)[i].engine();
                        }
                }
        }

private:
        /// Actual first-time seeding. Procedure varies according to engine type, see scpecialisations.
        void Seed(randutils::seed_seq_fe128& seseq)
        {
                auto seeds = pcg_extras::generate_one<unsigned long>(seseq);
                for (size_t i = 0; i < m_stream_count; ++i) {
                        (*this)[i].engine().seed(seeds);
                        (*this)[i].engine().split(m_stream_count, i);
                }
        }

private:
        std::string  m_seed_seq_init; ///< Seed sequence initializer used with engines.
        unsigned int m_stream_count;  ///< Number of threads/streams set up with the engine.
};

/// Specialization of seeding for pcg64.
template <>
inline void Rn<pcg64>::Seed(randutils::seed_seq_fe128& seseq)
{
        if (2 * m_stream_count > 64) {
                throw std::runtime_error("RnPcg64 generate seed vector, cannot handle large n.");
        }
        auto seeds = pcg_extras::generate_vector<pcg64::state_type, 64>(seseq);
        for (size_t i = 0; i < m_stream_count; ++i) {
                (*this)[i].engine().seed(seeds[i + 1], seeds[i]);
        }
}

extern template class Rn<pcg64>;
extern template class Rn<trng::lcg64>;

} // namespace util
} // namespace stride
