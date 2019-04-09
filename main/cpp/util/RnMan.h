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
 * Interface of RnMan.
 */

#pragma once

#include "RnInfo.h"

#include <functional>
#include <memory>
#include <vector>

namespace stride {
namespace util {

class RnEngine;

/*
 * RnMan manages random engines and distribution to produce random generators.
 * Can be used with (up to 32) parallel streams out of the engine.
 */
class RnMan
{
public:
        /// Default constructor builds empty (uninitialized) manager.
        RnMan();

        /// Initializing Constructor.
        explicit RnMan(const RnInfo& info);

        /// Equality of states
        bool operator==(const RnMan& other);

        /// Return the state of the random engines.
        RnInfo GetInfo() const;

        /// Return a generator for uniform doubles in [0, 1[ using i-th random stream.
        std::function<double()> GetUniform01Generator(unsigned int i = 0U);

        /// Return a generator for uniform ints in [a, b[ (a < b) using i-th random stream.
        std::function<int()> GetUniformIntGenerator(int a, int b, unsigned int i = 0U);

        /// Return generator for ints [0, n-1[ with non-negative weights p_j (i=0,..,n-1) using i-th random stream.
        std::function<int()> GetDiscreteGenerator(const std::vector<double>& weights, unsigned int i = 0U);

        /// Initalize with data in Info.
        void Initialize(const RnInfo& info);

        /// Is this een empty (i.e. non-initialized Rn)?
        bool IsEmpty() const;

        /// Random shuffle of vector of int indices using i-th random stream.
        void Shuffle(std::vector<unsigned int>& indices, unsigned int i);

private:
        std::shared_ptr<RnEngine> m_rn;
};

} // namespace util
} // namespace stride
