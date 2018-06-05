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
    : m_engines(info.m_stream_count, RnPcg::EngineType()), m_seed_seq_init(info.m_seed_seq_init),
      m_stream_count(info.m_stream_count)
{
        Seed(info);
}

bool RnPcg::CheckAllDigits(const string& s)
{
        bool status = true;
        for (const auto& e: s) {
                status = (status && isdigit(e));
                if (!status) break;
        }
        return status;
}

RnPcg::Info RnPcg::GetInfo() const
{
        Info         info;
        stringstream ss;
        for (auto& e : m_engines) {
                ss << e;
        }
        info.m_seed_seq_init = m_seed_seq_init;
        info.m_state         = ss.str();
        info.m_stream_count  = m_stream_count;
        return info;
}

void RnPcg::Seed(const Info& info)
{
        if (m_stream_count != info.m_stream_count) {
                throw runtime_error("RnManager> stream count does not match in Seed.");
        }
        m_seed_seq_init = info.m_seed_seq_init;

        auto state = info.m_state;
        if (state.empty()) {
                vector<string> seed_vec = Split(m_seed_seq_init, ",");
                vector<unsigned int> seed_seq_vec;
                for (const auto& e : seed_vec) {
                        if (!CheckAllDigits(e)) {
                                throw runtime_error("RnManager> Error in seeding definiton.");
                        }
                        seed_seq_vec.push_back(FromString<unsigned int>(e));
                }

                seed_seq_fe128 seseq(seed_seq_vec.begin(), seed_seq_vec.end());
                vector<pcg64::state_type> seed(2*m_stream_count);
                seseq.generate(seed.begin(), seed.end());

                for (size_t i = 0; i < m_stream_count; ++i) {
                        m_engines[i].seed(seed[i], seed[i+1]);
                }
        } else {
                stringstream ss(state);
                for (size_t i = 0; i < m_stream_count; ++i) {
                        ss >> m_engines[i];
                }
        }
}

} // namespace util
} // namespace stride
