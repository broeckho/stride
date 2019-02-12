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

#include "KdTree.h"
#include "Coordinate.h"
#include "Location.h"

#include <boost/geometry/algorithms/equals.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/strategies/geographic/distance.hpp>
#include <memory>

namespace gengeopop {

namespace geogrid_detail {

/// \ref KdTree for some more information on methods.
class KdTree2DPoint
{
public:
        ///
        explicit KdTree2DPoint(const std::shared_ptr<Location>& location)
            : m_pt(location->GetCoordinate()), m_location(location)
        {
        }

        ///
        KdTree2DPoint() : m_pt(), m_location(nullptr){};

        ///
        KdTree2DPoint(double longt, double lat) : m_pt(longt, lat), m_location(nullptr) {}

        ///
        constexpr static std::size_t dim = 2;

        ///
        template <std::size_t D>
        double Get() const
        {
                static_assert(0 <= D && D <= 1, "Dimension should be in range");
                return boost::geometry::get<D>(m_pt);
        }

        ///
        bool InBox(const AABB<KdTree2DPoint>& box) const
        {
                return boost::geometry::within(m_pt,
                                               boost::geometry::model::box<Coordinate>{box.lower.m_pt, box.upper.m_pt});
        }

        /// Does the point lie within `radius` km from `start`?
        bool InRadius(const KdTree2DPoint& start, double radius) const { return Distance(start) <= radius; }

        /// Retrieve the location.
        std::shared_ptr<Location> GetLocation() const { return m_location; }

        /// Get the coordinate for this Location.
        Coordinate GetPoint() const { return m_pt; }

        ///
        template <std::size_t D>
        struct dimension_type
        {
                using type = double;
        };

        ///
        bool operator==(const KdTree2DPoint& point) const
        {
                return Distance(point) < 0.001;
        }

private:
        Coordinate                m_pt;       ///< Shortcut for access without dereferencing.
        std::shared_ptr<Location> m_location; ///< The underlying location.

        /// Distance in kilometers, following great circle distance on a speroid earth.
        double Distance(const KdTree2DPoint& other) const
        {
                return boost::geometry::distance(m_pt, other.m_pt,
                                                 boost::geometry::strategy::distance::geographic<>{}) /
                       1000.0;
        }
};

} // namespace geogrid_detail
} // namespace gengeopop
