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

#include "geo/GeoGrid.h"

#include <memory>
#include <vector>

namespace stride {
namespace generator {

class GrahamScan
{
public:
        /// Initializing constructor.
        /// \param points       cloud of points.
        GrahamScan(std::vector<Point2D> points) : m_points(std::move(points)) {}

        /// Scan the points and return a vector of points which form the border of the points.
        std::vector<Point2D> Scan();

        /// Order a vector of points in polar order.
        struct PolarOrder
        {
                PolarOrder(const GrahamScan& g) : m_graham_scan(g) {}
                bool              operator()(const Point2D& p1, const Point2D& p2);
                const GrahamScan& m_graham_scan;
        };

private:
        /// Determine if the three points are counter-clockwise
        int ccw(Point2D p1, Point2D p2, Point2D p3) const;

        /// Return the square distance between two points.
        double SqrDist(Point2D p1, Point2D p2) const;

private:
        std::vector<Point2D> m_points; ///< A cloud of points
        Point2D              m_pivot;  ///< A point used as pivot in the Scan
};

} // namespace generator
} // namespace stride