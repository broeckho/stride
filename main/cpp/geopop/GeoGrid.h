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
#include "geopop/geo/GeoGridKdTree.h"

#include <set>
#include <unordered_map>
#include <vector>

namespace stride {
class ContactPool;
class Population;
} // namespace stride

namespace geopop {

class Location;

template <typename Policy, typename... F>
class GeoAggregator;

/**
 * A Geographic grid of simulation region contains Locations that in turn contain
 * an index to the ContactPools situated at that Location.
 */
class GeoGrid
{
public:
        /// GeoGrid and associated Population.
        explicit GeoGrid(stride::Population* population);

        /// No copy constructor.
        GeoGrid(const GeoGrid&) = delete;

        /// No copy assignment.
        GeoGrid operator=(const GeoGrid&) = delete;

        /// Adds a location to this GeoGrid.
        void AddLocation(std::shared_ptr<Location> location);

        /// Disables the addLocation method and builds the kdtree.
        void Finalize();

        /// Gets a Location by Id and check if the Id exists.
        std::shared_ptr<Location> GetById(unsigned int id) const { return m_locations[m_id_to_index.at(id)]; }

        /// Get the Population associated with this GeoGrid
        stride::Population* GetPopulation() const { return m_population; }

        /// Search for locations in \p radius (in km) around \p start.
        std::vector<const Location*> LocationsInRadius(const Location& start, double radius) const;

        /// Find contactpools in startRadius (in km) around start and, if none are found, double
        /// the radius and search again until the radius gets infinite. May return an empty vector
        /// when there are really no pools to be found (empty grid).
        std::vector<stride::ContactPool*> GetNearbyPools(stride::ContactType::Id id, const Location& start,
                                                         double startRadius = 10.0) const;

        /**
         * Gets the locations in a rectangle determined by the two coordinates (long1, lat1) and (long2, lat2).
         * The coordinates must be positioned on the diagonal, i.e:
         *
         *  p1 -----+     +-------p1
         *  |       |     |       |
         *  |       |  or |       |
         *  |       |     |       |
         *  +-------p2    p2------+
         */
        std::set<const Location*> LocationsInBox(double long1, double lat1, double long2, double lat2) const;

        /// Gets the location in a rectangle defined by the two Locations.
        std::set<const Location*> LocationsInBox(Location* loc1, Location* loc2) const;

        /// Gets the K biggest (in population count) locations of this GeoGrid
        std::vector<Location*> TopK(size_t k) const;

public:
        /// Build a GeoAggregator with a predefined functor and given args for the Policy.
        template <typename Policy, typename F>
        GeoAggregator<Policy, F> BuildAggregator(F functor, typename Policy::Args&& args) const;

        /// Build a GeoAggregator that gets its functor when calling, with given args for the Policy.
        template <typename Policy>
        GeoAggregator<Policy> BuildAggregator(typename Policy::Args&& args) const;

public:
        using iterator       = std::vector<std::shared_ptr<Location>>::iterator;
        using const_iterator = std::vector<std::shared_ptr<Location>>::const_iterator;

        /// Iterator to first Location.
        iterator begin() { return m_locations.begin(); }

        /// Iterator to the end of the Location storage.
        iterator end() { return m_locations.end(); }

        /// Const Iterator to first Location.
        const_iterator cbegin() const { return m_locations.cbegin(); }

        /// Const iterator to the end of the Location storage.
        const_iterator cend() const { return m_locations.cend(); }

        /// Gets a Location by index, doesn't performs a range check.
        std::shared_ptr<Location>& operator[](size_t index) { return m_locations[index]; }

        /// Gets a Location by index, doesn't performs a range check.
        const std::shared_ptr<Location>& operator[](size_t index) const { return m_locations[index]; }

        /// Gets current size of Location storage.
        size_t size() const { return m_locations.size(); }

private:
        ///< Checks whether the GeoGrid is finalized i.e. certain operations can(not) be used.
        void CheckFinalized(const std::string& functionName) const;

private:
        ///< Container for Locations in GeoGrid.
        std::vector<std::shared_ptr<Location>> m_locations;

        ///< Associative container maps Location Id to index in m_locations.
        std::unordered_map<unsigned int, unsigned int> m_id_to_index;

        ///< Stores pointer to Popluation, but does not take ownership.
        stride::Population* m_population;

        ///< Is the GeoGrid finalized (ready for use) yet?
        bool m_finalized;

        ///< Internal KdTree for quick spatial lookup.
        GeoGridKdTree m_tree;
};

} // namespace geopop
