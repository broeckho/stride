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
 * Calculate geographic coordinates.
 */

#include "geo/GeoCoordinate.h"

#include <cmath>
#include <iostream>
#include <random>

namespace stride {
namespace util {

using namespace std;

class GeoCoordCalculator
{
        /// Singleton pattern
public:
        static const GeoCoordCalculator& getInstance();

        double getDistance(const GeoCoordinate& coord1, const GeoCoordinate& coord2) const;
        /// Result is in kilometers
        /// Uses the haversine formula
        /// See: http://www.movable-type.co.uk/scripts/latlong.html

        template <class T>
        GeoCoordinate generateRandomCoord(const GeoCoordinate& coord, double radius, T rng) const
        {
                /// Partially the inverse of GeoCoordCalculator::getDistance, therefore I use the same variable names
                /// For future improvements, use this:
                /// http://gis.stackexchange.com/questions/25877/generating-random-locations-nearby
                double temp2 = radius / 6371;
                double temp1 = sin(temp2 / 2.0) * sin(temp2 / 2.0);

                double max_delta_latitude = asin(sqrt(temp1)) * 360.0 / PI;

                double my_cos = cos(coord.m_latitude * PI / 180.0);
                double my_pow = pow(my_cos, 2);
                double max_delta_longitude = asin(sqrt(temp1 / my_pow)) * 360.0 / PI;

                std::uniform_real_distribution<double> dist_longitude(-max_delta_longitude, max_delta_longitude);
                std::uniform_real_distribution<double> dist_latitude(-max_delta_latitude, max_delta_latitude);
                GeoCoordinate random_coordinate;

                do {
                        double new_longitude = coord.m_longitude + dist_longitude(rng);
                        double new_latitude = coord.m_latitude + dist_latitude(rng);

                        random_coordinate.m_longitude = new_longitude;
                        random_coordinate.m_latitude = new_latitude;
                        this->convertToRegularCoordinates(new_latitude, new_longitude);

                } while (getDistance(coord, random_coordinate) > radius);
                return random_coordinate;
        }
        /// radius is in kilometres
        /// TODO make the distribution fair

        GeoCoordinate getMiddle(const GeoCoordinate& coord1, const GeoCoordinate& coord2) const
        {
                /// Formula found online
                /// TODO test
                double lon1_rad = coord1.m_longitude * PI / 180;
                double lon2_rad = coord2.m_longitude * PI / 180;
                double lat1_rad = coord1.m_latitude * PI / 180;
                double lat2_rad = coord2.m_latitude * PI / 180;

                double lon_diff = lon2_rad - lon1_rad;
                double x = cos(lat2_rad) * cos(lon_diff);
                double y = cos(lat2_rad) * sin(lon_diff);

                double center_lat =
                    atan2(sin(lat1_rad) + sin(lat2_rad), sqrt((cos(lat1_rad) + x) * (cos(lat1_rad) + x) + y * y));
                double center_lon = lon1_rad + atan2(y, cos(lat1_rad) + x);

                return GeoCoordinate(center_lat * 180 / PI, center_lon * 180 / PI);
        }

        void convertToRegularCoordinates(double& latitude, double& longitude) const;

private:
        GeoCoordCalculator() {}

        ~GeoCoordCalculator() {}

        GeoCoordCalculator(GeoCoordCalculator const&) = delete;

        void operator=(GeoCoordCalculator const&) = delete;
};

} // namespace util
} // namespace stride
