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
 * Geographic coordinate implementation.
 */

#include "GeoCoordinate.h"

using namespace stride;
using namespace util;
using namespace std;

bool stride::util::operator==(const GeoCoordinate& coord1, const GeoCoordinate& coord2)
{
        return coord1.m_latitude == coord2.m_latitude && coord1.m_longitude == coord2.m_longitude;
}

bool stride::util::operator<(const GeoCoordinate& coord1, const GeoCoordinate& coord2)
{
        if (coord1.m_longitude < coord2.m_longitude) {
                return true;
        } else if (coord1.m_longitude > coord2.m_longitude) {
                return false;
        }

        return coord1.m_latitude < coord2.m_latitude;
}

std::ostream& stride::util::operator<<(std::ostream& os, const GeoCoordinate& g)
{
        os << "(LATITUDE: " << g.m_latitude << ", LONGITUDE: " << g.m_longitude << ")";
        return os;
}