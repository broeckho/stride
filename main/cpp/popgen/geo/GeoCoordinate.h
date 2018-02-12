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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Geographic coordinates.
 */
#include <iostream>

namespace stride {
namespace util {

#define PI 3.14159265

using namespace std;

struct GeoCoordinate
{
        // Note that it is required (precondition) to have valid long- and latitudes
        GeoCoordinate()
        {
                m_latitude = 0.0;
                m_longitude = 0.0;
        }

        GeoCoordinate(double lat, double lon)
        {
                m_latitude = lat;
                m_longitude = lon;
        }

        double m_longitude = 0.0;
        double m_latitude = 0.0;
};

bool operator==(const GeoCoordinate& coord1, const GeoCoordinate& coord2);

bool operator<(const GeoCoordinate& coord1, const GeoCoordinate& coord2);

std::ostream& operator<<(std::ostream& os, const GeoCoordinate& g);
}
} // namespace stride