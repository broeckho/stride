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

#include "GrahamScan.h"
#include <algorithm>
#include <cmath>

namespace stride {
namespace generator {

std::vector<Point2D> GrahamScan::Scan()
{
        std::vector<Point2D> hull;
        if (m_points.size() < 3) {
                return hull;
        }

        // Find location with least y coordinate
        int index = 0;
        for (unsigned int i = 1; i < m_points.size(); i++) {
                if (m_points[i] < m_points[index]) {
                        index = i;
                }
        }
        m_pivot = m_points[index];

        // Swap the pivot with the first point.
        Point2D temp    = m_points[0];
        m_points[0]     = m_points[index];
        m_points[index] = temp;

        // Sort the remaining points according to polar order.
        std::sort(m_points.begin() + 1, m_points.end(), PolarOrder(*this));

        hull.push_back(m_points[0]);
        hull.push_back(m_points[1]);
        hull.push_back(m_points[2]);

        for (unsigned int i = 3; i < m_points.size(); i++) {
                Point2D top = hull.back();
                hull.pop_back();

                while (this->ccw(hull.back(), top, m_points[i]) != -1) {
                        top = hull.back();
                        hull.pop_back();
                }

                hull.push_back(top);
                hull.push_back(m_points[i]);
        }

        return hull;
}

int GrahamScan::ccw(Point2D p1, Point2D p2, Point2D p3) const
{
        // Three points are a counter-clockwise turn if ccw > 0, clockwise if
        // ccw < 0, and collinear if ccw = 0. Because ccw is a determinant that
        // gives twice the signed area of the triangle formed by p1, p2 and p3.

        const double area = (p2.m_x - p1.m_x) * (p3.m_y - p1.m_y) - (p2.m_y - p1.m_y) * (p3.m_x - p1.m_x);
        if (area > 0)
                return -1;
        if (area < 0)
                return 1;
        return 0;
}

double GrahamScan::SqrDist(Point2D p1, Point2D p2) const
{
        double dx = p1.m_x - p2.m_x;
        double dy = p1.m_y - p2.m_y;
        return std::pow(dx, 2) + std::pow(dy, 2);
}

bool GrahamScan::PolarOrder::operator()(const Point2D& p1, const Point2D& p2)
{
        int order = m_graham_scan.ccw(m_graham_scan.m_pivot, p1, p2);
        if (order == 0) {
                return m_graham_scan.SqrDist(m_graham_scan.m_pivot, p1) <
                       m_graham_scan.SqrDist(m_graham_scan.m_pivot, p2);
        }
        return (order == -1);
}

} // namespace generator
} // namespace stride
