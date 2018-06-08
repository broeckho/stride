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

#include <functional>
#include <pcg/pcg_random.hpp>
#include <pcg/randutils.hpp>
#include <string>
#include <vector>

namespace stride {
namespace util {

/**
 * Manages random number generation in parallel (OpenMP) calculations.
 */
class RnPcg
{
public:
        using EngineType    = pcg64;
        using GeneratorType = randutils::random_generator<pcg64, randutils::seed_seq_fe128>;

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
        /// Initializes.
        explicit RnPcg(const Info& info = Info());

        /// No copying.
        RnPcg(const RnPcg&) = delete;

        /// No copy assignment.
        RnPcg& operator=(const RnPcg&) = delete;

        /// Equality of states
        bool operator==(const RnPcg& other);

        /// Return the generator engine for the ith thread.
        GeneratorType& operator[](size_t i) { return m_generators[i]; }

        /// Return the generator engine for the ith thread.
        const GeneratorType& operator[](size_t i) const { return m_generators[i]; }

        /// Return the generator engine for the ith thread.
        GeneratorType& at(size_t i) { return m_generators.at(i); }

        /// Return the generator engine for the ith thread.
        const GeneratorType& at(size_t i) const { return m_generators.at(i); }

public:
        /// Return the state of the random engine.
        Info GetInfo() const;

        /// Initalize with data in Info.
        void Seed(const Info& info = Info());

private:
        /// Check that the seed string is all digits.
        static bool CheckAllDigits(const std::string& seed);

private:
        std::vector<GeneratorType> m_generators;    ///< An random generator engine for each thread.
        std::string                m_seed_seq_init; ///< Actual seed sequence initializer used with engine.
        unsigned int               m_stream_count;  ///< Number of streams set up with the engine.
};

} // namespace util
} // namespace stride
