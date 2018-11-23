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

#pragma once

#include "ContactCenter.h"
#include "GeoGrid.h"

namespace gengeopop {

/// Models a K12School as ContactCenter
class K12School : public ContactCenter
{
public:
        explicit K12School(unsigned int id);
        std::string  GetType() const override;
        unsigned int GetMaxPools() const override;
        unsigned int GetPoolSize() const override;
        void         Fill(const std::shared_ptr<GeoGrid>& geoGrid) override;
};
} // namespace gengeopop
