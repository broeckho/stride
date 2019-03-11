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

#include <iostream>
#include <string>

namespace stride {
namespace util {

/**
 * Information on random number management state..
 */
struct RnInfo
{
        explicit RnInfo(std::string seed_seq_init = "1,2,3,4", std::string state = "", unsigned int stream_count = 1U)
            : m_seed_seq_init(std::move(seed_seq_init)), m_state(std::move(state)), m_stream_count(stream_count){};

        std::string  m_seed_seq_init; ///< Seed for the engine.
        std::string  m_state;         ///< Long string representing current state.
        unsigned int m_stream_count;  ///< Number of streams set up with the engine.
};

inline std::ostream& operator<<(std::ostream& os, const RnInfo& info)
{
        os << "Seed sequence: " << info.m_seed_seq_init << "\n"
           << "Number of streams: " << info.m_stream_count << "\n"
           << "State: " << info.m_state;
        return os;
}

} // namespace util
} // namespace stride
