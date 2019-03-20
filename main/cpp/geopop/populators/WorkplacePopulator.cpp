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

#include "WorkplacePopulator.h"

#include "contact/AgeBrackets.h"
#include "contact/ContactPool.h"
#include "geopop/GeoGrid.h"
#include "geopop/GeoGridConfig.h"
#include "geopop/Location.h"
#include "util/Assert.h"

#include <utility>

namespace geopop {

using namespace std;
using namespace stride;
using namespace stride::ContactType;
using namespace util;

WorkplacePopulator::WorkplacePopulator(RnMan& rnMan, shared_ptr<spdlog::logger> logger)
    : Populator(rnMan, move(logger)), m_geogrid_config(), m_wp_at_location(), m_nearby_wp(), m_gen_non_commute(),
    m_commuting_locations(), m_gen_commute()
{
}

void WorkplacePopulator::Apply(GeoGrid& geoGrid, const GeoGridConfig& geoGridConfig)
{
        m_logger->trace("Starting to populate Workplaces");

        m_geogrid_config  = geoGridConfig;
        m_gen_commute     = function<int()>();
        m_gen_non_commute = function<int()>();

        m_wp_at_location.clear();
        m_nearby_wp.clear();
        m_commuting_locations.clear();

        const auto fractionCommutingStudents = FractionCommutingStudents();
        WorkplacePoolsAtLocation(geoGrid);

        // for every location
        for (const auto& loc : geoGrid) {
                if (loc->GetPopCount() == 0) {
                        continue;
                }
                CommutingLocations(loc, fractionCommutingStudents);
                NearbyWorkspacePools(geoGrid, loc);

                // 2. for everyone of working age: decide between work or college (iff of College age)
                for (const auto& hhCenter : loc->RefCenters(Id::Household)) {
                        auto contactPool = (*hhCenter)[0];
                        for (auto p : *contactPool) {
                                if (AgeBrackets::Workplace::HasAge((p->GetAge()))) {
                                        bool isStudent      = MakeChoice(geoGridConfig.input.participation_college);
                                        bool isActiveWorker = MakeChoice(geoGridConfig.input.particpation_workplace);

                                        if ((AgeBrackets::College::HasAge(p->GetAge()) && !isStudent) ||
                                            isActiveWorker) {
                                                AssignActive(p);
                                        } else {
                                                // this person has no employment
                                                p->SetPoolId(Id::Workplace, 0);
                                        }
                                }
                        }
                }
        }

        m_logger->trace("Done populating Workplaces");
}

void WorkplacePopulator::AssignActive(Person* person)
{
        // this person is (student and active) or active
        if (!m_commuting_locations.empty() && MakeChoice(m_geogrid_config.input.fraction_workplace_commuters)) {
                // this person commutes
                const auto& info = m_wp_at_location[m_commuting_locations[m_gen_commute()]];
                const auto  id   = info.second(); // id of the location this person is commuting to
                info.first[id]->AddMember(person);
                person->SetPoolId(Id::Workplace, info.first[id]->GetId());
        } else {
                // this person does not
                const auto idraw = m_gen_non_commute();
                m_nearby_wp[idraw]->AddMember(person);
                person->SetPoolId(Id::Workplace, m_nearby_wp[idraw]->GetId());
        }
}

void WorkplacePopulator::CommutingLocations(const std::shared_ptr<Location> &loc, double fractionCommuteStudents)
{
        // find all Workplaces were employees from this location commute to
        m_commuting_locations.clear();
        m_gen_commute = function<int()>();

        vector<double> commutingWeights;
        for (const pair<Location*, double>& commute : loc->CRefOutgoingCommutes()) {
                const auto& workplaces = commute.first->RefCenters(Id::Workplace);
                if (!workplaces.empty()) {
                        m_commuting_locations.push_back(commute.first);
                        const auto weight = commute.second - (commute.second * fractionCommuteStudents);
                        commutingWeights.push_back(weight);
                        AssertThrow(weight >= 0.0 && weight <= 1.0 && !isnan(weight),
                                    "Invalid weight: " + to_string(weight), m_logger);
                }
        }

        if (!commutingWeights.empty()) {
                m_gen_commute = m_rn_man.GetDiscreteGenerator(commutingWeights, 0U);
        }
}

double WorkplacePopulator::FractionCommutingStudents()
{
        double fraction = 0.0;
        if (static_cast<bool>(m_geogrid_config.input.fraction_workplace_commuters) &&
            m_geogrid_config.popInfo.popcount_workplace) {
                fraction =
                    (m_geogrid_config.popInfo.popcount_college * m_geogrid_config.input.fraction_college_commuters) /
                    (m_geogrid_config.popInfo.popcount_workplace * m_geogrid_config.input.fraction_workplace_commuters);
        }
        return fraction;
}

void WorkplacePopulator::NearbyWorkspacePools(GeoGrid &geoGrid, std::shared_ptr<Location> loc)
{
        m_nearby_wp = GetNearbyPools(Id::Workplace, geoGrid, *loc);
        m_gen_non_commute = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(m_nearby_wp.size()), 0U);
}

void WorkplacePopulator::WorkplacePoolsAtLocation(GeoGrid &geoGrid)
{
        for (const auto& loc : geoGrid) {
                vector<ContactPool*> contactPools;
                for (const auto& wp : loc->RefCenters(Id::Workplace)) {
                        contactPools.insert(contactPools.end(), wp->begin(), wp->end());
                }

                auto disPools = m_rn_man.GetUniformIntGenerator(0, static_cast<int>(contactPools.size()), 0U);

                m_wp_at_location[loc.get()] = {contactPools, disPools};
        }
}

} // namespace geopop
