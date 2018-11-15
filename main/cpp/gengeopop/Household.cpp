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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include "Household.h"
#include "GeoGridConfig.h"

using namespace stride::ContactPoolType;

namespace gengeopop {

Household::Household() : ContactCenter(0) {}

Household::Household(unsigned int id) : ContactCenter(id) {}

unsigned int Household::GetMaxPools() const { return 1; }

unsigned int Household::GetPoolSize() const { return 15; }

std::string Household::GetType() const { return "Household"; }

void Household::Fill(const std::shared_ptr<GeoGrid>& geoGrid) { AddPool(geoGrid->CreateContactPool(Id::Household)); }
} // namespace gengeopop
