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

#include "Coordinate.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace geopop {

class ContactCenter;

/**
 * A location for use within the GeoGrid. Container for ContactCenters.
 */
class Location
{
public:
        /// Parametrized constructor without population count.
        Location(unsigned int id, unsigned int province, Coordinate coordinate = Coordinate(0.0, 0.0),
                 std::string name = "");

        /// Parametrized constructor with population count.
        Location(unsigned int id, unsigned int province, unsigned int popCount,
                 Coordinate coordinate = Coordinate(0.0, 0.0), std::string name = "");

        /// Perform a full comparison with the other location.
        bool operator==(const Location& other) const;

        /// Add a ContactCenter.
        template <typename T>
        void AddContactCenter(std::shared_ptr<T> contactCenter)
        {
                m_CC.push_back(contactCenter);
                m_CC_OfType[typeid(T)].push_back(contactCenter);
        }

        /// Adds a Location and a proportion to the incoming commute vector.
        /// I.e. fraction of commuting population at otherLocation commuting to this Location.
        void AddIncomingCommutingLocation(std::shared_ptr<Location> otherLocation, double fraction);

        /// Adds a Location and a fraction to the outgoing commute vector.
        // I.e. fraction of commuting population at this Location commuting to otherLocation.
        void AddOutgoingCommutingLocation(std::shared_ptr<Location> otherLocation, double fraction);

        /// Gets the Coordinate of this Location.
        const Coordinate& GetCoordinate() const { return m_coordinate; }

        /// Gets all ContactCenters at this location.
        const std::vector<std::shared_ptr<ContactCenter>>& GetContactCenters() const { return m_CC; }

        /// Gets the Contact Centers of a specific type (Household, Workplace, ...).
        template <typename T>
        std::vector<std::shared_ptr<ContactCenter>> GetContactCentersOfType()
        {
                return m_CC_OfType[typeid(T)];
        }

        /// Gets a vector with the outgoing cities which people are commuting to + the proportion.
        const std::vector<std::pair<Location*, double>>& GetIncomingCommuningCities() const;

        /// Gets ID of this Location.
        unsigned int GetID() const { return m_id; }

        /// Calculates number of incomming commuters, given the fraction of the population that commutes.
        int GetIncomingCommuterCount(double fractionCommuters) const;

        /// Gets the amount of people infected in the contactpools of this location.
        double GetInfectedCount() const;

        /// Gets the name
        std::string GetName() const { return m_name; }

        /// Returns outgoing cities which people are commuting to + the proportion.
        const std::vector<std::pair<Location*, double>>& GetOutgoingCommutingCities() const;

        /// Calculates number of outgoing commuters, given the fraction of the population that commutes.
        int GetOutgoingCommuterCount(double fractionCommuters) const;

        /// Gets the absolute population
        unsigned int GetPopCount() const { return m_pop_count; }

        /// Gets the province
        unsigned int GetProvince() const { return m_province; }

        /// Gets the relative population
        double GetRelativePopulationSize() const;

        /// Sets the Coordinate of this Socation.
        void SetCoordinate(const Coordinate& coordinate) { m_coordinate = coordinate; }

        /// Calculates this location's population count using its relative population and the total population count.
        void SetPopCount(unsigned int totalPopCount);

        /// Sets the relative population, which will be later used by @see CalculatePopulation.
        void SetRelativePopulation(double relativePopulation);

public:
        /// To iterate over container with ContactCenters at this Location.
        using iterator = std::vector<std::shared_ptr<ContactCenter>>::iterator;

        /// Gets iterator to the first ContacCenter at this Location.
        iterator begin() { return m_CC.begin(); }

        /// Gets iterator to the end of the ContactCenters at this Location.
        iterator end() { return m_CC.end(); }

private:
        unsigned int m_id = 0;       ///< Id.
        std::string  m_name;         ///< Name.
        unsigned int m_province;     ///< Province id.
        unsigned int m_pop_count;    ///< Population count (number of individuals) at this Location.
        double       m_pop_fraction; ///< Fraction of whole population at thois Location.
        Coordinate   m_coordinate;   ///< Coordinate of the Location.

        ///< All contactCenters at this Location.
        std::vector<std::shared_ptr<ContactCenter>> m_CC;

        /// Incomming commutes stored as pair of Location and fraction of population at that Location.
        std::vector<std::pair<Location*, double>> m_inCommuteLocations;

        ///< Outgoing commutes stored as pair of Location and fraction of population to this this Location.
        std::vector<std::pair<Location*, double>> m_outCommuteLocations;

        ///< Stores the contact centers indexed by their type.
        std::unordered_map<std::type_index, std::vector<std::shared_ptr<ContactCenter>>> m_CC_OfType;
};

} // namespace geopop
