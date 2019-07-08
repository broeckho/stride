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

#include "RnInfo.h"

#include <trng/discrete_dist.hpp>
#include <trng/lcg64.hpp>
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>
#include <functional>
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

        using ContainerType::operator[];
        using ContainerType::at;
        using ContainerType::size;

public:
        /// Default constructor build empty manager.
        Rn() : ContainerType(), m_seed_seq_init(""), m_stream_count(0U) {}

        /// Initializes.
        explicit Rn(const RnInfo& info)
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
        RnInfo GetInfo() const;

        /// Return a generator for uniform doubles in [0, 1[ using i-th random engine.
        std::function<double()> GetUniform01Generator(unsigned int i = 0U)
        {
                return ContainerType::at(i).variate_generator(trng::uniform01_dist<double>());
        }

        /// Return a generator for uniform ints in [a, b[ (a < b) using i-th random engine.
        std::function<int()> GetUniformIntGenerator(int a, int b, unsigned int i = 0U)
        {
                return ContainerType::at(i).variate_generator(trng::uniform_int_dist(a, b));
        }

        /// Return generator for integers [0, n-1[ with non-negative weights p_j (i=0,..,n-1) using i-th random engine.
        //std::function<int()> GetDiscreteGenerator(const std::vector<double>& weights, unsigned int i = 0U)
        //{
        //        return ContainerType::at(i).variate_generator(trng::discrete_dist(weights.begin(), weights.end()));
        //}

        /// Return generator for integers [0, n-1[ with non-negative weights p_j (i=0,..,n-1) using i-th random engine.
        template<typename It>
        std::function<int()> GetDiscreteGenerator(It begin, It end, unsigned int i = 0U)
        {
                return ContainerType::at(i).variate_generator(trng::discrete_dist(begin, end));
        }

        /// Initalize with data in Info.
        void Initialize(const RnInfo& info);

        /// Is this een empty (i.e. non-initialized Rn)?
        bool IsEmpty() const { return ContainerType::empty() || (m_stream_count == 0U); }

        /// Random shuffle of vector of unsigned int indices using i-th engine.
        void Shuffle(std::vector<unsigned int>& indices, unsigned int i)
        {
                ContainerType::at(i).shuffle(indices.begin(), indices.end());
        }

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
extern template class Rn<trng::lcg64>;

} // namespace util
} // namespace stride
