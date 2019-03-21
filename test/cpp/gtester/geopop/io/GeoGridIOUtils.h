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

#include "geopop/ContactCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/Location.h"

namespace proto {
class GeoGrid;
class GeoGrid_Location;
class GeoGrid_Location_Coordinate;
class GeoGrid_Location_ContactCenter;
class GeoGrid_Location_ContactCenter_ContactPool;
class GeoGrid_Person;
} // namespace proto

namespace stride {
class ContactPool;
class Population;
} // namespace stride

using namespace geopop;

void CompareContactPool(stride::ContactPool*                                     contactPool,
                        const proto::GeoGrid_Location_ContactCenter_ContactPool& protoContactPool);

void CompareContactCenter(const ContactCenter&                         contactCenter,
                          const proto::GeoGrid_Location_ContactCenter& protoContactCenter);

void CompareCoordinate(const Coordinate& coordinate, const proto::GeoGrid_Location_Coordinate& protoCoordinate);

void CompareLocation(const Location& location, const proto::GeoGrid_Location& protoLocation);

void ComparePerson(const proto::GeoGrid_Person& protoPerson);

void CompareGeoGrid(GeoGrid& geoGrid);

void CompareGeoGrid(proto::GeoGrid& protoGrid);

std::shared_ptr<GeoGrid> GetPopulatedGeoGrid(stride::Population*);

std::shared_ptr<GeoGrid> GetCommutesGeoGrid(stride::Population*);
