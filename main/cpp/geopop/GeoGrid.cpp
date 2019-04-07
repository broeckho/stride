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

#include "GeoGrid.h"

#include "geopop/Location.h"
#include "geopop/geo/GeoAggregator.h"
#include "geopop/geo/GeoGridKdTree.h"

#include <queue>
#include <stdexcept>
#include <utility>

namespace geopop {

using namespace std;

GeoGrid::GeoGrid(stride::Population* population)
    : m_locations(), m_locationsToIdIndex(), m_population(population), m_finalized(false), m_tree()
{
}

void GeoGrid::AddLocation(shared_ptr<Location> location)
{
        if (m_finalized) {
                throw std::runtime_error("Calling addLocation while GeoGrid is finalized not supported!");
        }
        m_locations.emplace_back(location);
        m_locationsToIdIndex[location->GetID()] = location;
}

template <typename Policy, typename F>
GeoAggregator<Policy, F> GeoGrid::BuildAggregator(F functor, typename Policy::Args&& args) const
{
        return GeoAggregator<Policy, F>(m_tree, functor, std::forward<typename Policy::Args>(args));
}

template <typename Policy>
GeoAggregator<Policy> GeoGrid::BuildAggregator(typename Policy::Args&& args) const
{
        return GeoAggregator<Policy>(m_tree, std::forward<typename Policy::Args>(args));
}

void GeoGrid::CheckFinalized(const string& functionName) const
{
        if (!m_finalized) {
                throw std::runtime_error("Calling \"" + functionName + "\" with GeoGrid not finalized not supported!");
        }
}

stride::ContactPool* GeoGrid::CreateContactPool(stride::ContactType::Id type)
{
        return m_population->CreateContactPool(type);
}

void GeoGrid::Finalize()
{
        vector<geogrid_detail::KdTree2DPoint> points;
        for (const auto& loc : m_locations) {
                points.emplace_back(geogrid_detail::KdTree2DPoint(loc));
        }
        m_tree      = GeoGridKdTree::Build(points);
        m_finalized = true;
}

set<shared_ptr<Location>> GeoGrid::LocationsInBox(double long1, double lat1, double long2, double lat2) const
{
        CheckFinalized(__func__);

        set<shared_ptr<Location>> result;

        auto agg = BuildAggregator<BoxPolicy>(
            MakeCollector(inserter(result, result.begin())),
            make_tuple(min(long1, long2), min(lat1, lat2), max(long1, long2), max(lat1, lat2)));
        agg();

        return result;
}

std::set<std::shared_ptr<Location>> GeoGrid::LocationsInBox(const std::shared_ptr<Location>& loc1,
                                                            const std::shared_ptr<Location>& loc2) const
{
        using boost::geometry::get;
        return LocationsInBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                              get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
}

vector<shared_ptr<Location>> GeoGrid::LocationsInRadius(shared_ptr<Location> start, double radius) const
{
        CheckFinalized(__func__);

        geogrid_detail::KdTree2DPoint startPt(start);
        vector<shared_ptr<Location>>  result;

        auto agg =
            BuildAggregator<RadiusPolicy>(MakeCollector(back_inserter(result)), make_tuple(move(startPt), radius));
        agg();

        return result;
}

vector<shared_ptr<Location>> GeoGrid::TopK(size_t k) const
{
        auto cmp = [](const shared_ptr<Location>& rhs, const shared_ptr<Location>& lhs) {
                return rhs->GetPopCount() > lhs->GetPopCount();
        };

        priority_queue<shared_ptr<Location>, vector<shared_ptr<Location>>, decltype(cmp)> queue(cmp);
        for (const auto& loc : m_locations) {
                queue.push(loc);
                if (queue.size() > k) {
                        queue.pop();
                }
        }

        vector<shared_ptr<Location>> topLocations;
        while (!queue.empty()) {
                auto loc = queue.top();
                topLocations.push_back(loc);
                queue.pop();
        }

        return topLocations;
}

void GeoGrid::Remove(const shared_ptr<Location>& location)
{
        m_locations.erase(::geopop::remove(m_locations.begin(), m_locations.end(), location), m_locations.end());
        m_locationsToIdIndex.erase(location->GetID());
}

} // namespace geopop
