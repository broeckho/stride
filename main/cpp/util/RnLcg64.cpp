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
 * Implementation of RnLcg64.
 */

#include "RnLcg64.h"
#include "StringUtils.h"

#include <cctype>
#include <pcg/pcg_random.hpp>
#include <sstream>

using namespace std;
using namespace randutils;

namespace stride {
namespace util {

RnLcg64::RnLcg64(const Info& info)
    : ContainerType(info.m_stream_count), m_seed_seq_init(info.m_seed_seq_init), m_stream_count(info.m_stream_count)
{
        Seed(info);
}

bool RnLcg64::operator==(const RnLcg64& other)
{
        bool status = m_stream_count == other.m_stream_count;
        if (status) {
                for (size_t i = 0; i < size(); ++i) {
                        status = status && ((*this)[i] == other[i]);
                }
        }
        return status;
}

RnLcg64::Info RnLcg64::GetInfo() const
{
        Info         info;
        stringstream ss;
        for (auto& e : *this) {
                ss << e.engine() << " "; // space as a separator
        }
        info.m_seed_seq_init = m_seed_seq_init;
        info.m_state         = ss.str();
        info.m_stream_count  = m_stream_count;
        return info;
}

void RnLcg64::Seed(const Info& info)
{
        if (m_stream_count != info.m_stream_count) {
                throw runtime_error("RnLcg64::Seed> stream count does not match in Seed.");
        }
        m_seed_seq_init = info.m_seed_seq_init;

        auto state = info.m_state;
        if (state.empty()) {
                vector<unsigned int> seseq_init_vec;
                for (const auto& e : Split(m_seed_seq_init, ",")) {
                        if (!CheckAllDigits(e)) {
                                throw runtime_error("RnLcg64::Seed> Error in seeding definiton.");
                        }
                        seseq_init_vec.push_back(FromString<unsigned int>(e));
                }

                seed_seq_fe128 seseq(seseq_init_vec.begin(), seseq_init_vec.end());
                auto           seeds = pcg_extras::generate_one<unsigned long>(seseq);

                for (size_t i = 0; i < m_stream_count; ++i) {
                        (*this)[i].engine().seed(seeds);
                        (*this)[i].engine().split(m_stream_count, i);
                }
        } else {
                stringstream ss(state);
                for (size_t i = 0; i < m_stream_count; ++i) {
                        ss >> (*this)[i].engine();
                        (*this)[i].engine().split(m_stream_count, i);
                }
        }
}

} // namespace util
} // namespace stride
