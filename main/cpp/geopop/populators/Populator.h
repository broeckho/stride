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

#include "contact/ContactType.h"
#include "contact/ContactPool.h"
#include "geopop/GeoGridConfig.h"
#include "util/LogUtils.h"
#include "util/RnMan.h"

#include <spdlog/logger.h>

namespace geopop {

class GeoGrid;
class GeoGridConfig;
class Location;

/**
 * Populator uses geo & pop data to populate ContactPools in the GeoGrid.
 */
template <typename stride::ContactType::Id ID>
class Populator
{
public:
        /// Construct with a RnMan and a logger.
        explicit Populator(stride::util::RnMan& rnMan, std::shared_ptr<spdlog::logger> logger = nullptr)
                : m_rn_man(rnMan), m_logger(move(logger))
        {
                if (!m_logger)
                        m_logger = stride::util::LogUtils::CreateNullLogger();
        }

        /// Default is OK.
        ~Populator() = default;

        /// Populate the ContactPools type ID. This is a placeholder for the specializations.
        void Apply(GeoGrid&, const GeoGridConfig&) {};

protected:
        stride::util::RnMan&            m_rn_man; ///< RnManager used by populators.
        std::shared_ptr<spdlog::logger> m_logger; ///< Logger used by populators.
};

// ---------------------------------------------------------------
// Declare specializations (implemntation in separate .cpp files).
// ---------------------------------------------------------------
template<>
void Populator<stride::ContactType::Id::K12School>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig);

template<>
void Populator<stride::ContactType::Id::College>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig);

template<>
void Populator<stride::ContactType::Id::Workplace>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig);

template<>
void Populator<stride::ContactType::Id::Household>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig);

template<>
void Populator<stride::ContactType::Id::PrimaryCommunity>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig);

template<>
void Populator<stride::ContactType::Id::SecondaryCommunity>::Apply(GeoGrid& geoGrid, const GeoGridConfig& ggConfig);

// ---------------------------------------------------------------
// Shorthand definitions.
// ---------------------------------------------------------------
using K12SchoolPopulator = Populator<stride::ContactType::Id::K12School>;
using CollegePopulator = Populator<stride::ContactType::Id::College>;
using WorkplacePopulator = Populator<stride::ContactType::Id::Workplace>;
using HouseholdPopulator = Populator<stride::ContactType::Id::Household>;
using PrimaryCommunityPopulator = Populator<stride::ContactType::Id::PrimaryCommunity>;
using SecondaryCommunityPopulator = Populator<stride::ContactType::Id::SecondaryCommunity>;

} // namespace geopop
