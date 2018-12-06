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

#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace gengeopop {

using Coordinate = boost::geometry::model::point<double, 2, boost::geometry::cs::geographic<boost::geometry::degree>>;

class ContactCenter;

/// A location for use within the GeoGrid
/// Contains useful information, both spatial and administrative
class Location
{
public:
        ///
        Location(unsigned int id, unsigned int province, Coordinate coordinate = Coordinate(0.0, 0.0),
                 std::string name = "");

        Location(unsigned int id, unsigned int province, unsigned int population,
                 Coordinate coordinate = Coordinate(0.0, 0.0), std::string name = "");

        /// Perform a full compare of the given locations.
        bool operator==(const Location& other) const;

        /// Add a ContactCenter
        template <typename T>
        void AddContactCenter(std::shared_ptr<T> contactCenter)
        {
                m_contactCenters.push_back(contactCenter);
                m_contactCenterByType[typeid(T)].push_back(contactCenter);
        }

        /// Gets the Contact Centers of a specific type \p T
        template <typename T>
        std::vector<std::shared_ptr<ContactCenter>> GetContactCentersOfType()
        {
                return m_contactCenterByType[typeid(T)];
        }

        /// Gets the name
        std::string GetName() const;

        /// Gets the province
        unsigned int GetProvince() const;

        /// Gets the ID
        unsigned int GetID() const;

        /// Gets the absolute population
        unsigned int GetPopulation() const;

        /// Gets the population amount used in the simulation
        unsigned int GetSimulationPopulation() const;

        /// Gets the ratio of infected persons in all contactPools of this location
        double GetInfectedRatio() const;

        /// Gets the amount of people infected in the contactpools of this location
        double GetInfectedCount() const;

        /// Given the total population calculates this location's population using the relative population of this
        /// location
        void CalculatePopulation(unsigned int totalPopulation);

        /// Sets the relative population, which will be later used by @see CalculatePopulation
        void SetRelativePopulation(double relativePopulation);

        /// Gets the relative population
        double GetRelativePopulationSize() const;

        /// Gets a vector with the outgoing cities which people are commuting to + the proportion
        const std::vector<std::pair<Location*, double>>& GetIncomingCommuningCities() const;

        /// Adds a Location and a proportion to the incoming commutng vector
        /// I.e. \p proportion of the commuting population in \p location are commuting to \p this
        void AddIncomingCommutingLocation(std::shared_ptr<Location> location, double proportion);

        /// @return a vector with the outgoing cities which people are commuting to + the proportion
        const std::vector<std::pair<Location*, double>>& GetOutgoingCommuningCities() const;

        /// Adds a Location and a proportion to the incoming commuting vector
        // I.e. \p proportion of the commuting population in \p this are commuting to \p location
        void AddOutgoingCommutingLocation(std::shared_ptr<Location> location, double proportion);

        /// Gets the absolute amount of poeple leaving from this location
        int OutGoingCommutingPeople(double fractionOfPopulationCommuting) const;

        /// Gets the absolute amount of people going to this location
        int IncomingCommutingPeople(double fractionOfPopulationCommuting) const;

        /// Gets the Coordinate of this location
        const Coordinate& GetCoordinate() const;
        void              SetCoordinate(const Coordinate& coordinate);


        /// Gets all contact centers at this location
        const std::vector<std::shared_ptr<ContactCenter>>& GetContactCenters() const;

public:
        using iterator = std::vector<std::shared_ptr<ContactCenter>>::iterator;

        /// Gets iterator to the first location
        iterator begin();

        /// Gets iterator to the end of the locations
        iterator end();

private:
        unsigned int m_id = 0;             ///< Id
        std::string  m_name;               ///< Name
        unsigned int m_province;           ///< Province id
        unsigned int m_population;         ///< The absolute population
        double       m_relativePopulation; ///< The relative population (relative against whole population)
        Coordinate   m_coordinate;         ///< Coordinate
        std::vector<std::shared_ptr<ContactCenter>> m_contactCenters; ///< All contactCenters
        /// Incomming commutes stored as pair of location and proportion relative to the given location
        std::vector<std::pair<Location*, double>> m_incomingCommutingLocations;
        ///< Outgoing commutes stored as pair of location and proportion relative to the this location
        std::vector<std::pair<Location*, double>> m_outgoingCommutingLocations;
        std::unordered_map<std::type_index, std::vector<std::shared_ptr<ContactCenter>>>
            m_contactCenterByType; ///< Stores the contact centers indexed by their type
};

} // namespace gengeopop
