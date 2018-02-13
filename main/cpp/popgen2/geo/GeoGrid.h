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

#include <tuple>

namespace stride {
namespace generator {

enum PointType
{
        XY,
        LONGLAT,
        Null
};

class Point2D
{
public:
        ///
        Point2D(double x = 0.0, double y = 0.0, PointType point_type = PointType::Null)
            : m_x(x), m_y(y), m_point_type(point_type)
        {
        }

        ///
        bool operator==(const Point2D& other) const;

        ///
        bool operator!=(const Point2D& other) const;

        ///
        bool operator<(const Point2D& other) const;

        double    m_x, m_y;
        PointType m_point_type;
};

/// Convert degrees to radians
Point2D DegToRad(Point2D degrees);

/// Convert radians to degrees
Point2D RadToDeg(Point2D radians);

/// Determine the bounding box around a longitude/latitude point with a given distance.
std::tuple<Point2D, Point2D> BoundingBox(Point2D origin, double distance);

/// Sort to points in ascending x-coordinate order.
bool AscendingXCoord(const Point2D& lhs, const Point2D& rhs);

double distance(const Point2D& lhs, const Point2D& rhs);

} // namespace generator
} // namespace stride
