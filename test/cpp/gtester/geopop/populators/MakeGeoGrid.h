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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
/**
 * Builds a GeoGrid in the population with all Locations at Coordinate (0.0, 0.0), all having
 * the same population count, the same number of K12 schools, the same number of Households,
 * each having the same number of Persons.
 *
 * @param ggConfig          The GeoGridConfig to use.
 * @param locCount          The number of Locations.
 * @param locPop            The population count at each Location.
 * @param schoolCount       The number of K12Schools at each Location.
 * @param houseHoldCount    The number of households at each Location.
 * @param personCount       The number of persons per Household.
 * @param pop               The population carrying this GeoGrid.
 */
void MakeGeoGrid(const geopop::GeoGridConfig& ggConfig, int locCount, int locPop, int schoolCount, int houseHoldCount,
                 int personCount, stride::Population* pop);
