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
 * Calculate geographic coordinate, implementations.
 */

#include "GeoCoordCalculator.h"

using namespace stride;
using namespace util;
using namespace std;

double earth_radius = 6371;

const GeoCoordCalculator& GeoCoordCalculator::getInstance()
{
        static GeoCoordCalculator calc;
        return calc;
}

double GeoCoordCalculator::getDistance(const GeoCoordinate& coord1, const GeoCoordinate& coord2) const
{
        double delta_latitude  = coord2.m_latitude - coord1.m_latitude;
        double delta_longitude = coord2.m_longitude - coord1.m_longitude;

        double temp1 = sin(delta_latitude * PI / 360.0) * sin(delta_latitude * PI / 360.0) +
                       cos(coord1.m_latitude * PI / 180.0) * cos(coord2.m_latitude * PI / 180.0) *
                           sin(delta_longitude * PI / 360.0) * sin(delta_longitude * PI / 360.0);

        double temp2 = 2.0 * asin(min(1.0, sqrt(temp1)));

        return earth_radius * temp2;
}

void GeoCoordCalculator::convertToRegularCoordinates(double& latitude, double& longitude) const
{
        bool changed = false;
        while (latitude > 270) {
                latitude -= 360;
        }
        while (latitude < -270) {
                latitude += 360;
        }
        while (longitude > 180) {
                longitude -= 360;
        }
        while (longitude < -180) {
                longitude += 360;
        }
        if (latitude < -90.0) {
                latitude += 180;
                latitude = -latitude;
                longitude += 180;
                changed = true;
        }
        if (latitude > 90) {
                latitude -= 180;
                latitude = -latitude;
                longitude += 180;
                changed = true;
        }
        if (longitude < -180.0) {
                changed = true;
        }
        if (longitude > 180) {
                longitude -= 180.0;
                changed = true;
        }
        if (changed) {
                convertToRegularCoordinates(latitude, longitude);
        }
}