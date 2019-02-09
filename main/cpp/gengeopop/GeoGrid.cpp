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

#include "GeoGrid.h"

#include "util/Exception.h"

#include <cmath>
#include <iostream>
#include <queue>
#include <utility>

namespace gengeopop {

using namespace std;

GeoGrid::GeoGrid(stride::Population* population)
    : m_locations(), m_locationsToIdIndex(), m_population(population), m_finalized(false), m_tree()

{
}

void GeoGrid::AddLocation(shared_ptr<Location> location)
{
        if (m_finalized) {
                throw stride::util::Exception("Calling addLocation while GeoGrid is finalized is not supported!");
        }

        m_locations.emplace_back(location);
        m_locationsToIdIndex[location->GetID()] = location;
}

shared_ptr<Location> GeoGrid::operator[](size_t index) { return *(begin() + index); }

shared_ptr<Location> GeoGrid::Get(size_t index) { return (*this)[index]; }

vector<shared_ptr<Location>> GeoGrid::TopK(size_t k) const
{
        auto cmp = [](const shared_ptr<Location>& rhs, const shared_ptr<Location>& lhs) {
                return rhs->GetPopCount() > lhs->GetPopCount();
        };

        priority_queue<shared_ptr<Location>, vector<shared_ptr<Location>>, decltype(cmp)> queue(
            cmp);

        for (auto it = cbegin(); it != cend(); it++) {
                queue.push(*it);
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

size_t GeoGrid::size() const { return m_locations.size(); }

shared_ptr<Location> GeoGrid::GetById(unsigned int id) { return m_locationsToIdIndex.at(id); }

void GeoGrid::remove(const shared_ptr<Location>& location)
{
        m_locations.erase(::gengeopop::remove(m_locations.begin(), m_locations.end(), location), m_locations.end());
        m_locationsToIdIndex.erase(location->GetID());
}

void GeoGrid::Finalize()
{
        vector<geogrid_detail::KdTree2DPoint> points;
        for (auto it = begin(); it != end(); ++it) {
                points.emplace_back(geogrid_detail::KdTree2DPoint(*it));
        }

        m_finalized = true;
        m_tree      = KdTree<geogrid_detail::KdTree2DPoint>::Build(points);
}

set<shared_ptr<Location>> GeoGrid::InBox(double long1, double lat1, double long2, double lat2) const
{
        CheckFinalized(__func__);

        set<shared_ptr<Location>> result;
        auto agg = BuildAggregator<BoxPolicy>(MakeCollector(inserter(result, result.begin())),
                                              make_tuple(min(long1, long2), min(lat1, lat2),
                                                              max(long1, long2), max(lat1, lat2)));
        agg();
        return result;
}

vector<shared_ptr<Location>> GeoGrid::FindLocationsInRadius(shared_ptr<Location> start,
                                                                      double                    radius) const
{
        CheckFinalized(__func__);

        geogrid_detail::KdTree2DPoint startPt(start);

        vector<shared_ptr<Location>> result;
        auto agg = BuildAggregator<RadiusPolicy>(MakeCollector(back_inserter(result)),
                                                 make_tuple(move(startPt), radius));
        agg();
        return result;
}

stride::Population* GeoGrid::GetPopulation() { return m_population; }

void GeoGrid::CheckFinalized(const string& functionName) const
{
        if (!m_finalized) {
                throw stride::util::Exception("Calling \"" + functionName +
                                              "\" while GeoGrid is not finalized is not supported!");
        }
}

stride::ContactPool* GeoGrid::CreateContactPool(stride::ContactPoolType::Id type)
{
        return m_population->CreateContactPool(type);
}

} // namespace gengeopop
