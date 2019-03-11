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
 *  Copyright 2018, 2019, Jan Broeckhove and Bistromatics group.
 */

#include "Household.h"

#include "GeoGrid.h"
#include "GeoGridConfig.h"

using namespace stride::ContactType;

namespace geopop {

void Household::Fill(const GeoGridConfig& geoGridConfig, const std::shared_ptr<GeoGrid>& geoGrid)
{
        for (auto i = 0U; i < geoGridConfig.pools.pools_per_houselhold; ++i) {
                const auto p = geoGrid->CreateContactPool(stride::ContactType::Id::Household);
                RegisterPool(p);
        }
}

} // namespace geopop
