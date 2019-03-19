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

#include "KdTree2DPoint.h"

#include "geopop/Location.h"

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/core/access.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/strategies/geographic/distance.hpp>

namespace geopop {

namespace geogrid_detail {

KdTree2DPoint::KdTree2DPoint(const Location* loc) : m_pt(loc->GetCoordinate()), m_location(loc) {}

bool KdTree2DPoint::operator==(const KdTree2DPoint& other) const { return Distance(other) < 0.001; }

bool KdTree2DPoint::InBox(const AABBox<KdTree2DPoint>& box) const
{
        return boost::geometry::within(m_pt, boost::geometry::model::box<Coordinate>{box.lower.m_pt, box.upper.m_pt});
}

bool KdTree2DPoint::InRadius(const KdTree2DPoint& start, double radius) const { return Distance(start) <= radius; }

double KdTree2DPoint::Distance(const KdTree2DPoint& other) const
{
        return boost::geometry::distance(m_pt, other.m_pt, boost::geometry::strategy::distance::geographic<>{}) /
               1000.0;
}

} // namespace geogrid_detail
} // namespace geopop
