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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

#include "AliasMethod.h"

namespace stride {
namespace generator {

/// Appends a linear interpolation of probability values for all unsigned integers in [begin, end].
template <typename T>
void AddRangeLinear(AliasMethod<T>& aliasmethod, T begin, T end, double prob_begin, double prob_end)
{
        assert(end >= begin);
        double probability_step = (prob_end - prob_begin) / (end - begin);
        for (T i = begin; i <= end; ++i) {
                aliasmethod.Add(i, prob_begin + i * probability_step);
        }
}

} // namespace generator
} // namespace stride