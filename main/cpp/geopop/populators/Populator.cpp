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

#include "Populator.h"

#include "contact/ContactPool.h"
#include "geopop/CollegeCenter.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/HouseholdCenter.h"
#include "geopop/K12SchoolCenter.h"
#include "geopop/Location.h"
#include "geopop/PrimaryCommunityCenter.h"
#include "geopop/SecondaryCommunityCenter.h"
#include "geopop/WorkplaceCenter.h"
#include "util/LogUtils.h"

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;

Populator::Populator(util::RnMan& rnMan, shared_ptr<spdlog::logger> logger) : m_rn_man(rnMan), m_logger(move(logger))
{
        if (!m_logger)
                m_logger = stride::util::LogUtils::CreateNullLogger();
}

bool Populator::MakeChoice(double fraction)
{
        vector<double> weights;
        weights.push_back(1.0 - fraction); // -> 0, return is false -> not part of the fraction
        weights.push_back(fraction);       // -> 1, return is true -> part of the fraction

        auto dist = m_rn_man.GetDiscreteGenerator(weights, 0U);
        return static_cast<bool>(dist());
}

vector<ContactPool*> Populator::GetNearbyPools(Id id, const GeoGrid& geoGrid, const Location& start,
                                               double startRadius) const
{
        double               currentRadius = startRadius;
        vector<ContactPool*> pools;

        while (pools.empty()) {
                for (const Location* nearLoc : geoGrid.LocationsInRadius(start, currentRadius)) {
                        const auto& centers = nearLoc->CRefCenters(id);
                        for (const auto& center : centers) {
                                pools.insert(pools.end(), center->begin(), center->end());
                        }
                }
                currentRadius *= 2;
                if (currentRadius == numeric_limits<double>::infinity()) {
                        break;
                }
        }
        return pools;
}

} // namespace geopop
