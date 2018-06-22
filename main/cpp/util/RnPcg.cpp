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
 * Implementation of RnPcg.
 */

#include "RnPcg.h"
#include "StringUtils.h"

#include <cctype>
#include <sstream>

using namespace std;
using namespace randutils;

namespace stride {
namespace util {

RnPcg::RnPcg(const Info& info)
    : ContainerType(info.m_stream_count), m_seed_seq_init(info.m_seed_seq_init), m_stream_count(info.m_stream_count)
{
        Seed(info);
}

bool RnPcg::operator==(const RnPcg& other)
{
        bool status = m_stream_count == other.m_stream_count;
        if (status) {
                for (size_t i = 0; i < size(); ++i) {
                        status = status && ((*this)[i] == other[i]);
                }
        }
        return status;
}

bool RnPcg::CheckAllDigits(const string& s)
{
        bool status = true;
        for (const auto& e : s) {
                status = (status && isdigit(e));
                if (!status)
                        break;
        }
        return status;
}

RnPcg::Info RnPcg::GetInfo() const
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

void RnPcg::Seed(const Info& info)
{
        if (m_stream_count != info.m_stream_count) {
                throw runtime_error("RnPcg::Seed> stream count does not match in Seed.");
        }
        m_seed_seq_init = info.m_seed_seq_init;

        auto state = info.m_state;
        if (state.empty()) {
                vector<unsigned int> seseq_init_vec;
                for (const auto& e : Split(m_seed_seq_init, ",")) {
                        if (!CheckAllDigits(e)) {
                                throw runtime_error("RnPcg::Seed> Error in seeding definiton.");
                        }
                        seseq_init_vec.push_back(FromString<unsigned int>(e));
                }

                seed_seq_fe128 seseq(seseq_init_vec.begin(), seseq_init_vec.end());
                auto           seeds = generate_seed_vector<pcg64::state_type>(2 * m_stream_count, seseq);

                for (size_t i = 0; i < m_stream_count; ++i) {
                        (*this)[i].engine().seed(seeds[i + 1], seeds[i]);
                }
        } else {
                stringstream ss(state);
                for (size_t i = 0; i < m_stream_count; ++i) {
                        ss >> (*this)[i].engine();
                }
        }
}

} // namespace util
} // namespace stride
