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

#include "GeoGrid.h"
#include <cmath>
#include <iostream>

namespace stride {
namespace generator {

const double EARTH_RADIUS = 6371.0; // in km.

bool Point2D::operator==(const Point2D& other) const
{
        return m_x == other.m_x && m_y == other.m_y && m_point_type == other.m_point_type;
}

bool Point2D::operator!=(const Point2D& other) const
{
        return !(m_x == other.m_x && m_y == other.m_y && m_point_type == other.m_point_type);
}

bool Point2D::operator<(const Point2D& other) const
{
        if (m_y != other.m_y)
                return m_y < other.m_y;
        return m_x < other.m_x;
}

Point2D DegToRad(Point2D degrees)
{
        double rad_x = degrees.m_x * M_PI / 180.0;
        double rad_y = degrees.m_y * M_PI / 180.0;
        return {rad_x, rad_y, degrees.m_point_type};
}

Point2D RadToDeg(Point2D radians)
{
        double deg_x = 180.0 * radians.m_x / M_PI;
        double deg_y = 180.0 * radians.m_y / M_PI;
        return {deg_x, deg_y, radians.m_point_type};
}

std::tuple<Point2D, Point2D> BoundingBox(Point2D origin, double distance)
{
        origin = DegToRad(origin);

        double lat_min = origin.m_y - (distance / EARTH_RADIUS);
        double lat_max = origin.m_y + (distance / EARTH_RADIUS);
        double lon_min = origin.m_x - (distance / EARTH_RADIUS / std::cos(lat_min));
        double lon_max = origin.m_x + (distance / EARTH_RADIUS / std::cos(lat_max));

        Point2D lon_lat_min = RadToDeg(Point2D(lon_min, lat_min, PointType::LONGLAT));
        Point2D lon_lat_max = RadToDeg(Point2D(lon_max, lat_max, PointType::LONGLAT));

        return std::make_tuple(lon_lat_min, lon_lat_max);
};

bool AscendingXCoord(const Point2D& lhs, const Point2D& rhs) { return lhs.m_x <= rhs.m_x; }

double distance(const Point2D& lhs, const Point2D& rhs)
{
        if (lhs.m_point_type != rhs.m_point_type)
                throw std::invalid_argument("stride::generator::distance> Wrong point types");

        double result = 0;

        if (lhs.m_point_type == PointType::XY) {
                result = std::sqrt(std::pow(lhs.m_x - rhs.m_x, 2) + std::pow(lhs.m_y - rhs.m_y, 2));
        } else if (lhs.m_point_type == PointType::LONGLAT) {
                Point2D me  = DegToRad(lhs);
                Point2D you = DegToRad(rhs);

                double d_lat = me.m_y - you.m_y;
                double d_lon = me.m_x - you.m_x;

                // This uses the Haversine formula
                double a = std::sin(d_lat / 2) * std::sin(d_lat / 2) +
                           std::cos(me.m_y) * std::cos(you.m_y) * std::sin(d_lon / 2) * std::sin(d_lon / 2);
                double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
                double d = EARTH_RADIUS * c; // Distance in km
                result   = d * 1000;         // Distance in meters
        } else {
                throw std::invalid_argument("stride::generator::distance> No distance for PointType::Null");
        }

        return result;
}

} // namespace generator
} // namespace stride
