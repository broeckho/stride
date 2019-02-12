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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include <iostream>

namespace gengeopop {

/**
 * Axis Aligned Bounding Box
 *
 * @brief A hyper rectangle defined by 2 points: the lower bound for every dimension and the upper bound.
 */
template <typename P>
struct AABB
{
        AABB() : lower(), upper(){};
        AABB(P l, P u) : lower(l), upper(u){};
        P lower; ///< The lower bound for every dimension
        P upper; ///< The upper bound for every dimension
};

} // namespace gengeopop
