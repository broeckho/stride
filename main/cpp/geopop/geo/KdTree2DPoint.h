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

#include "AABBox.h"
#include "geopop/Coordinate.h"

#include <boost/geometry/core/access.hpp>
#include <memory>

namespace geopop {

class Location;

namespace geogrid_detail {

/// \ref KdTree for some more information on methods.
class KdTree2DPoint
{
public:
        ///
        constexpr static std::size_t dim = 2;

        ///
        template <std::size_t D>
        struct dimension_type
        {
                using type = double;
        };

public:
        /// Default constructor.
        KdTree2DPoint() : m_pt(), m_location(nullptr){};

        /// Constructor with Location.
        explicit KdTree2DPoint(const Location* loc);

        /// Constructor with longitude and latitude.
        KdTree2DPoint(double longt, double lat) : m_pt(longt, lat), m_location(nullptr) {}

        /// Equal if within one meter of one another.
        bool operator==(const KdTree2DPoint& other) const;

        /// Distance in kilometers, following great circle distance on a speroid earth.
        double Distance(const KdTree2DPoint& other) const;

        ///
        template <std::size_t D>
        double Get() const
        {
                static_assert(0 <= D && D <= 1, "Dimension should be in range");
                return boost::geometry::get<D>(m_pt);
        }

        /// Retrieve the location.
        const Location* GetLocation() const { return m_location; }

        /// Get the coordinate for this Location.
        Coordinate GetPoint() const { return m_pt; }

        ///
        bool InBox(const AABBox<KdTree2DPoint>& box) const;

        /// Does the point lie within `radius` km from `start`?
        bool InRadius(const KdTree2DPoint& start, double radius) const;

private:
        Coordinate      m_pt;       ///< Shortcut for access without dereferencing.
        const Location* m_location; ///< The underlying location.
};

} // namespace geogrid_detail
} // namespace geopop
