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

#include "Rn.h"
#include "StringUtils.h"

#include <cctype>
#include <randutils/randutils.hpp>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace randutils;

namespace stride {
namespace util {

template <typename E>
bool Rn<E>::operator==(const Rn& other)
{
        bool status = m_stream_count == other.m_stream_count;
        if (status) {
                for (size_t i = 0; i < size(); ++i) {
                        status = status && ((*this)[i] == other[i]);
                }
        }
        return status;
}

template <typename E>
RnInfo Rn<E>::GetInfo() const
{
        RnInfo            info;
        std::stringstream ss;
        for (auto& e : *this) {
                ss << e.engine();
        }
        info.m_seed_seq_init = m_seed_seq_init;
        info.m_state         = ss.str();
        info.m_stream_count  = m_stream_count;
        return info;
}

template <typename E>
void Rn<E>::Initialize(const RnInfo& info)
{
        if (m_stream_count != info.m_stream_count) {
                m_stream_count = info.m_stream_count;
                this->resize(m_stream_count);
        }
        m_seed_seq_init = info.m_seed_seq_init;

        auto state = info.m_state;
        if (state.empty()) {
                std::vector<unsigned int> seseq_init_vec;
                const auto string_vec{Split(m_seed_seq_init, ",")};
                for (const auto& e : Split(m_seed_seq_init, ",")) {
                        if (!CheckAllDigits(e)) {
                                throw std::runtime_error("Rn::Seed> Error in seeding definiton: " + e);
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

template <>
void Rn<pcg64>::Seed(randutils::seed_seq_fe128& seseq)
{
        if (2 * m_stream_count > 64) {
                throw std::runtime_error("RnPcg64 generate seed vector, cannot handle large n.");
        }
        auto seeds = pcg_extras::generate_vector<pcg64::state_type, 64>(seseq);
        for (size_t i = 0; i < m_stream_count; ++i) {
                (*this)[i].engine().seed(seeds[i + 1], seeds[i]);
        }
}

template <typename E>
void Rn<E>::Seed(randutils::seed_seq_fe128& seseq)
{
        auto seeds = pcg_extras::generate_one<unsigned long>(seseq);
        for (size_t i = 0; i < m_stream_count; ++i) {
                (*this)[i].engine().seed(seeds);
                (*this)[i].engine().split(m_stream_count, i);
        }
}

template class Rn<pcg64>;
template class Rn<trng::lcg64>;

} // namespace util
} // namespace stride
