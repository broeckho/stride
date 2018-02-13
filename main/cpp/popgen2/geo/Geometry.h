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

#include "GeoGrid.h"

#include <cmath>
#include <limits>
#include <vector>

namespace stride {
namespace generator {

struct Line2D
{
        Line2D(unsigned int p0, unsigned int p1);
        unsigned int m_p0;
        unsigned int m_p1;
};

struct Polygon
{
        std::vector<Point2D> m_points;
        std::vector<Line2D>  m_lines;

        /// Determine the height of a line.
        double GetLineHeight(unsigned int lineIndex, double x);

        /// Add a point to the polygon
        unsigned int AddPoint(Point2D p);

        ///
        unsigned int AddPoint(double x, double y);

        /// Add a line to the polygon between the points with the indices p0 and p1.
        unsigned int AddLine(unsigned int p0, unsigned int p1);
};

class ConvexPolygonChecker
{
public:
        /// Builds a table to check if a point falls within the polygon
        explicit ConvexPolygonChecker(Polygon& poly);

        /// Check if a point is located in the polygon
        bool IsPointInPolygon(Point2D);

        ///
        bool IsPointInPolygon(double x, double y);

        ///
        struct Column
        {
                double       m_begin_x   = 0.0;
                double       m_end_x     = 0.0;
                unsigned int m_top       = 0U;
                unsigned int m_bottom    = 0U;
                bool         assignedTop = false;
                bool         IsInside(double x) const { return x >= m_begin_x && x <= m_end_x; }
        };

        ///
        std::vector<Column> GetColumns() { return m_columns; }

        double m_min_x;
        double m_min_y;
        double m_max_x;
        double m_max_y;

private:
        Polygon*            m_poly = nullptr;
        std::vector<Column> m_columns;
};

} // namespace generator
} // namespace stride