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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace geopop {
namespace kd {

template <typename P, std::size_t D>
std::size_t Median(const std::vector<P>& points)
{
        if (points.empty())
                return 0;

        using C = std::pair<decltype(points[0].template Get<D>()), std::size_t>;
        std::vector<C> sorting;
        for (std::size_t i = 0; i < points.size(); i++) {
                sorting.emplace_back(points[i].template Get<D>(), i);
        }
        std::sort(sorting.begin(), sorting.end());
        return sorting[sorting.size() / 2].second;
}

} // namespace kd
} // namespace geopop
