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
 * Implementation of RNManager.
 */

#include "RNManager.h"

#include <sstream>

using namespace std;
using namespace stride::util;
using namespace stride::RNEngineType;

namespace stride {
namespace util {

// This function is used as a quick utility routine
// Push the combined state of the engine streams to a stringstream.
template <typename T>
void para_info(stringstream& ss, const vector<T>& engines)
{
        for (const auto& e : engines) {
                ss << e;
        }
}

// This function is used as a quick utility routine.
// Seed a vector of random number engines and split them in parallel streams.
template <typename T>
void para_seed(vector<T>& engines, unsigned int stream_count, unsigned long seed)
{
        engines.resize(stream_count);
        for (size_t i = 0; i < stream_count; ++i) {
                engines[i].seed(seed);
                engines[i].split(stream_count, i);
        }
}

// This function is used as a quick utility routine
// Seed the state of a vector random number engines and split them in parallel streams.
template <typename T>
void para_seed(vector<T>& engines, unsigned int stream_count, const string& state)
{
        engines.resize(stream_count);
        stringstream ss(state);
        for (size_t i = 0; i < stream_count; ++i) {
                ss >> engines[i];
                engines[i].split(stream_count, i);
        }
}

RNManager::RNManager(const Info& info)
    : m_seed(), m_stream_count(), m_type_id(), m_lcg64(), m_lcg64_shift(), m_mrg2(), m_mrg3(), m_yarn2(), m_yarn3()
{
        Initialize(info);
}

RNManager::Info RNManager::GetInfo() const
{
        Info         info;
        stringstream ss;

        switch (m_type_id) {
        case Id::lcg64: para_info(ss, m_lcg64); break;
        case Id::lcg64_shift: para_info(ss, m_lcg64_shift); break;
        case Id::mrg2: para_info(ss, m_mrg2); break;
        case Id::mrg3: para_info(ss, m_mrg3); break;
        case Id::yarn2: para_info(ss, m_yarn2); break;
        case Id::yarn3: para_info(ss, m_yarn3); break;
        }

        info.m_seed         = m_seed;
        info.m_state        = ss.str();
        info.m_stream_count = m_stream_count;
        info.m_type         = ToString(m_type_id);
        return info;
}

void RNManager::Initialize(const Info& info)
{
        m_seed         = info.m_seed;
        m_stream_count = info.m_stream_count;
        m_type_id      = ToType(info.m_type);

        if (info.m_state.empty()) {
                switch (m_type_id) {
                case Id::lcg64: para_seed(m_lcg64, m_stream_count, m_seed); break;
                case Id::lcg64_shift: para_seed(m_lcg64_shift, m_stream_count, m_seed); break;
                case Id::mrg2: para_seed(m_mrg2, m_stream_count, m_seed); break;
                case Id::mrg3: para_seed(m_mrg3, m_stream_count, m_seed); break;
                case Id::yarn2: para_seed(m_yarn2, m_stream_count, m_seed); break;
                case Id::yarn3: para_seed(m_yarn3, m_stream_count, m_seed); break;
                }
        } else {
                switch (m_type_id) {
                case Id::lcg64: para_seed(m_lcg64, m_stream_count, info.m_state); break;
                case Id::lcg64_shift: para_seed(m_lcg64_shift, m_stream_count, info.m_state); break;
                case Id::mrg2: para_seed(m_mrg2, m_stream_count, info.m_state); break;
                case Id::mrg3: para_seed(m_mrg3, m_stream_count, info.m_state); break;
                case Id::yarn2: para_seed(m_yarn2, m_stream_count, info.m_state); break;
                case Id::yarn3: para_seed(m_yarn3, m_stream_count, info.m_state); break;
                }
        }
}

} // namespace util
} // namespace stride
