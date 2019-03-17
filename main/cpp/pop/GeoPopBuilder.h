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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Initialize populations.
 */

#pragma once

#include "AbstractPopBuilder.h"

#include <boost/property_tree/ptree_fwd.hpp>

#include <memory>

namespace geopop {
class GeoGrid;
class GeoGridConfig;
} // namespace geopop

namespace stride {

class Population;

/**
 * Builds poulation of a geographic grid.
 */
class GeoPopBuilder : public AbstractPopBuilder
{
public:
        /// Use constructor of base.
        using AbstractPopBuilder::AbstractPopBuilder;

        /// Generates a synthetic population.
        std::shared_ptr<Population> Build(std::shared_ptr<Population> pop) override;

private:
        /// Reads the data files.
        void MakeLocations(const std::shared_ptr<geopop::GeoGrid>& geoGrid, const geopop::GeoGridConfig& geoGridConfig,
                           const std::string& citiesFileName, const std::string& commutingFileName);

        /// Build and store the Geo part of the GeoGrid.
        void MakeCenters(const std::shared_ptr<geopop::GeoGrid>& geoGrid, const geopop::GeoGridConfig& geoGridConfig);

        /// Build and store the Pop part of the GeoGrid.
        void MakePersons(const std::shared_ptr<geopop::GeoGrid>& geoGrid, const geopop::GeoGridConfig& geoGridConfig);

private:
        /// The current number of ContactCenters, used to obtain an Id for a new contactCenter.
        /// ! 0 has special meaning (not assigned)!
        unsigned int m_ccCounter = 1;
};

} // namespace stride
