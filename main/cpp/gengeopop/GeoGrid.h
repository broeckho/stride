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

#include <cmath>
#include <iostream>
#include <map>
#include <pop/Population.h>
#include <set>
#include <unordered_map>
#include <vector>

#include "../pool/ContactPool.h"
#include "KdTree.h"
#include "Location.h"

// Trying to include specific headers to lower compilation times
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/strategies/geographic/distance.hpp>

namespace gengeopop {
template <typename Policy, typename... F>
class GeoAggregator;

namespace geogrid_detail {

/// Used as a point in de geogrid when using the KdTree
using BoostPoint = boost::geometry::model::point<double, 2, boost::geometry::cs::geographic<boost::geometry::degree>>;

/// \ref KdTree for some more information on methods
class KdTree2DPoint
{
public:
        explicit KdTree2DPoint(const std::shared_ptr<Location>& location)
            : m_pt(location->GetCoordinate()), m_location(location)
        {
        }

        KdTree2DPoint() : m_pt(), m_location(nullptr){};

        KdTree2DPoint(double longt, double lat) : m_pt(longt, lat), m_location(nullptr) {}

        constexpr static std::size_t dim = 2;

        template <std::size_t D>
        double Get() const
        {
                static_assert(0 <= D && D <= 1, "Dimension should be in range");
                return boost::geometry::get<D>(m_pt);
        }

        bool InBox(const AABB<KdTree2DPoint>& box) const
        {
                return boost::geometry::within(m_pt,
                                               boost::geometry::model::box<Coordinate>{box.lower.m_pt, box.upper.m_pt});
        }

        /// Does the point lie within `radius` km from `start`?
        bool InRadius(const KdTree2DPoint& start, double radius) const { return Distance(start) <= radius; }

        /// Retrieve the actual location
        std::shared_ptr<Location> GetLocation() const { return m_location; }

        /// Get the coordinate for this Location
        Coordinate GetPoint() const { return m_pt; }

        template <std::size_t D>
        struct dimension_type
        {
                using type = double;
        };

private:
        Coordinate                m_pt;       ///< Shortcut for access without dereferencing
        std::shared_ptr<Location> m_location; ///< The underlying location

        /// Distance in kilometers, following great circle distance on a speroid earth
        double Distance(const KdTree2DPoint& other) const
        {
                return boost::geometry::distance(m_pt, other.m_pt,
                                                 boost::geometry::strategy::distance::geographic<>{}) /
                       1000.0;
        }
};

} // namespace geogrid_detail

/// A Geographic information grid for a single region
class GeoGrid
{
public:
        using iterator       = std::vector<std::shared_ptr<Location>>::iterator;
        using const_iterator = std::vector<std::shared_ptr<Location>>::const_iterator;

        /// Construct with information about population
        GeoGrid(stride::Population* population);

        /// No copy constructor
        GeoGrid(const GeoGrid&) = delete;

        /// No copy assignment
        GeoGrid operator=(const GeoGrid&) = delete;

        /// Adds a location to this GeoGrid
        void AddLocation(std::shared_ptr<Location> location);

        /// Disables the addLocation method and builds the kdtree.
        void Finalize();

        /// Search for locations in \p radius around \p start
        std::vector<std::shared_ptr<Location>> FindLocationsInRadius(std::shared_ptr<Location> start,
                                                                     double                    radius) const;

        /// Gets the K biggest Location of this GeoGrid
        std::vector<std::shared_ptr<Location>> TopK(size_t k) const;

        /**
         * Gets the locations in a rectangle determined by the two coordinates (long1, lat1) and (long2, lat2).
         *
         * The coordinates must be position on the diagonal, i.e:
         *
         *  p1 -----+     +-------p1
         *  |       |     |       |
         *  |       |  or |       |
         *  |       |     |       |
         *  +-------p2    p2------+
         */
        std::set<std::shared_ptr<Location>> InBox(double long1, double lat1, double long2, double lat2) const;

        /// Gets the location in a rectangle defined by the two locations
        std::set<std::shared_ptr<Location>> InBox(const std::shared_ptr<Location>& loc1,
                                                  const std::shared_ptr<Location>& loc2) const
        {
                using boost::geometry::get;
                return InBox(get<0>(loc1->GetCoordinate()), get<1>(loc1->GetCoordinate()),
                             get<0>(loc2->GetCoordinate()), get<1>(loc2->GetCoordinate()));
        }

        /// Iterator to first Location
        iterator begin();

        /// Iterator to the end of the Location storage
        iterator end();

        /// Const Iterator to first Location
        const_iterator cbegin() const;

        /// Const iterator to the end of the Location storage
        const_iterator cend() const;

        /// Gets amount of Location
        size_t size() const;

        /// Remove element of GeoGrid
        void remove(const std::shared_ptr<Location>& location);

        /// Gets a Location by index, doesn't performs a range check
        std::shared_ptr<Location> operator[](size_t index);

        /// Gets a Location by index, doesn't performs a range check
        std::shared_ptr<Location> Get(size_t index);

        /// Gets a Location by id and check if the id exists
        std::shared_ptr<Location> GetById(unsigned int id);

        /// Gets the region name of this GeoGrid
        std::string GetRegionName() const;

        /// Create and store a Person in the GeoGrid and return a pointer to it, which works until deletion of the
        /// GeoGrid
        template <typename... Args>
        stride::Person* CreatePerson(Args&&... args)
        {
                return m_population->CreatePerson(args...);
        }

        /// Create a ContactPool of the given type and return a non-owning pointer
        stride::ContactPool* CreateContactPool(stride::ContactPoolType::Id type);

        /// Get the population of this GeoGrid
        stride::Population* GetPopulation();

        /// Build a GeoAggregator with a predefined functor and given args for the Policy
        template <typename Policy, typename F>
        GeoAggregator<Policy, F> BuildAggregator(F functor, typename Policy::Args&& args) const;

        /// Build a GeoAggregator that gets its functor when calling, with given args for the Policy
        template <typename Policy>
        GeoAggregator<Policy> BuildAggregator(typename Policy::Args&& args) const;

private:
        void CheckFinalized(const std::string& functionName)
            const; ///< Checks whether the GeoGrid is finalized and thus certain operations can(not) be used

        std::vector<std::shared_ptr<Location>> m_locations; ///< Locations in this geoGrid
        std::unordered_map<unsigned int, std::shared_ptr<Location>>
                            m_locationsToIdIndex; ///< Locations in this geoGrid indexed by Id
        stride::Population* m_population;         ///< Stores, but does not take ownership

        bool m_finalized; ///< Is this finalized yet?

        KdTree<geogrid_detail::KdTree2DPoint> m_tree; ///< Internal KdTree for quick spatial lookup
};

} // namespace gengeopop

#include "GeoAggregator.h" // Prevent cyclic include dependency

namespace gengeopop {

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

} // namespace gengeopop
