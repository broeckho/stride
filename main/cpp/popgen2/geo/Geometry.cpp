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
 *  Copyright 2017, Draulans S
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Geometry implementations.
 */

#include "Geometry.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

namespace stride {
namespace generator {

Line2D::Line2D(unsigned int p0, unsigned int p1) : m_p0(p0), m_p1(p1) {}

unsigned int Polygon::AddPoint(Point2D p)
{
        m_points.push_back(p);
        return static_cast<unsigned int>(m_points.size() - 1);
}

unsigned int Polygon::AddPoint(double x, double y)
{
        Point2D p(x, y, PointType::Null);
        m_points.push_back(p);
        return static_cast<unsigned int>(m_points.size() - 1);
}

unsigned int Polygon::AddLine(unsigned int p0, unsigned int p1)
{
        Line2D l(p0, p1);
        m_lines.push_back(l);
        return static_cast<unsigned int>(m_lines.size() - 1);
}

double Polygon::GetLineHeight(unsigned int lineIndex, double x)
{
        double x0 = m_points[m_lines[lineIndex].m_p0].m_x;
        double y0 = m_points[m_lines[lineIndex].m_p0].m_y;
        double x1 = m_points[m_lines[lineIndex].m_p1].m_x;
        double y1 = m_points[m_lines[lineIndex].m_p1].m_y;
        return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

ConvexPolygonChecker::ConvexPolygonChecker(Polygon& poly)
{
        m_poly = &poly;

        m_min_x = std::numeric_limits<double>::infinity();
        m_min_y = std::numeric_limits<double>::infinity();
        m_max_x = -std::numeric_limits<double>::infinity();
        m_max_y = -std::numeric_limits<double>::infinity();

        std::vector<Point2D*> point_pointers;

        // Preprocess points.
        for (Point2D& point : poly.m_points) {
                m_min_x = std::min(point.m_x, m_min_x);
                m_max_x = std::max(point.m_x, m_max_x);
                m_min_y = std::min(point.m_y, m_min_y);
                m_max_y = std::max(point.m_y, m_max_y);

                point_pointers.push_back(&point);
        }

        // Sort point pointers by their point's x position.
        std::sort(point_pointers.begin(), point_pointers.end(),
                  [](Point2D* p0, Point2D* p1) { return p0->m_x < p1->m_x; });

        // In the sorted list of points create columns for every range between unique x values.
        for (unsigned int i = 1; i < point_pointers.size(); ++i) {
                Point2D *p0 = point_pointers[i - 1], *p1 = point_pointers[i];
                if (p0->m_x == p1->m_x) {
                        continue;
                } // Test uniqueness of x values.

                Column col;
                col.m_begin_x = std::min(p0->m_x, p1->m_x);
                col.m_end_x   = std::max(p0->m_x, p1->m_x);
                m_columns.push_back(col);
        }

        // Now for every line check which columns are intersected by it.
        for (unsigned int lineId = 0; lineId < poly.m_lines.size(); lineId++) {
                Line2D& line      = poly.m_lines[lineId];
                double  leftmost  = std::min(poly.m_points[line.m_p0].m_x, poly.m_points[line.m_p1].m_x);
                double  rightmost = std::max(poly.m_points[line.m_p0].m_x, poly.m_points[line.m_p1].m_x);

                for (Column& col : m_columns) {
                        if (leftmost <= col.m_begin_x && rightmost >= col.m_end_x) {
                                // If the line crosses our column, it must be part of this column's check.
                                if (!col.assignedTop) {
                                        // No line has been assigned yet, so we can safely assign this one.
                                        col.m_top       = lineId;
                                        col.assignedTop = true;
                                } else {
                                        double& top_y0    = poly.m_points[poly.m_lines[col.m_top].m_p0].m_y;
                                        double& top_y1    = poly.m_points[poly.m_lines[col.m_top].m_p1].m_y;
                                        double  top_y_max = std::max(top_y0, top_y1);
                                        double  top_y_min = std::min(top_y0, top_y1);
                                        double  y_max =
                                            std::max(poly.m_points[line.m_p0].m_y, poly.m_points[line.m_p1].m_y);
                                        double y_min =
                                            std::min(poly.m_points[line.m_p0].m_y, poly.m_points[line.m_p1].m_y);

                                        if (top_y_max >= y_max && top_y_min >= y_min) {
                                                // The previously assigned line is indeed the top one.. (We can do this
                                                // test because it's a convex polygon.) This would also handle the case
                                                // where two overlapping horizontal lines would exist. (Only if there's
                                                // just 2 points.)
                                                col.m_bottom = lineId;
                                        } else if (top_y_max <= y_max && top_y_min <= y_min) {
                                                // The previously assigned line wasn't the top one. Move it to the
                                                // bottom & put this one as the top one.
                                                col.m_bottom = col.m_top;
                                                col.m_top    = lineId;
                                        } else {
                                                // Panic.
                                                throw std::runtime_error("Internal error while building convex polygon "
                                                                         "membership lookup table.");
                                        }
                                }
                        }
                }
        }

        // Ready for lookups.
}

bool ConvexPolygonChecker::IsPointInPolygon(double x, double y)
{
        auto status = false;
        if (!(x < m_min_x || x > m_max_x || y < m_min_y || y > m_max_y)) {
                // Binary search-ish for columns
                auto         left  = 0U;
                auto         right = static_cast<unsigned int>(m_columns.size());
                unsigned int middle;
                while (true) {
                        // Check if in middle
                        middle      = (left + right) / 2;
                        Column& col = m_columns[middle];
                        if (col.IsInside(x)) {
                                break;
                        } else if (x > col.m_end_x) {
                                left = middle + 1;
                        } else if (x < col.m_begin_x) {
                                right = middle - 1;
                        }
                }

                // Middle should now contain the column number to check, so check top & bottom of this column.
                status = (m_poly->GetLineHeight(m_columns[middle].m_top, x) >= y &&
                          m_poly->GetLineHeight(m_columns[middle].m_bottom, x) <= y);
        }
        return status;
}

bool ConvexPolygonChecker::IsPointInPolygon(Point2D point) { return IsPointInPolygon(point.m_x, point.m_y); }

} // namespace generator
} // namespace stride
