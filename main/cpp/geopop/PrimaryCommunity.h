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

#pragma once

#include "ContactCenter.h"

namespace geopop {

class GeoGrid;

/**
 * A model of a Primary Community (as a ContactCenter)
 */
class PrimaryCommunity : public ContactCenter
{
public:
        /// Construct community with assigned ID.
        explicit PrimaryCommunity(unsigned int id) : ContactCenter(id) {}

        /// See ContactCenter::Fill.
        void Fill(const GeoGridConfig& geoGridConfig, const std::shared_ptr<GeoGrid>& geoGrid) override;

        /// See ContactCenter::GetContactPoolType.
        stride::ContactType::Id GetContactPoolType() const override
        {
                return stride::ContactType::Id::PrimaryCommunity;
        }
};

} // namespace geopop
