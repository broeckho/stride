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
#include "contact/IdSubscriptArray.h"
#include "geopop/Coordinate.h"
#include "geopop/ContactCenter.h"

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
        void AddContactCenter(const std::shared_ptr<ContactCenter>& contactCenter)
        {
                m_CC_OfType[contactCenter->GetContactPoolType()].push_back(contactCenter);
        }

        /// Adds a Location and a proportion to the incoming commute vector.
        /// I.e. fraction of commuting population at otherLocation commuting to this Location.
        void AddIncomingCommutingLocation(std::shared_ptr<Location> otherLocation, double fraction);

        /// Adds a Location and a fraction to the outgoing commute vector.
        // I.e. fraction of commuting population at this Location commuting to otherLocation.
        void AddOutgoingCommutingLocation(std::shared_ptr<Location> otherLocation, double fraction);

        /// Gets the Coordinate of this Location.
        const Coordinate& GetCoordinate() const { return m_coordinate; }

        /// Gets the Contact Centers of a specific type (Household, Workplace, ...).
        std::vector<std::shared_ptr<ContactCenter>>& GetContactCentersOfType(stride::ContactType::Id id)
        {
                return m_CC_OfType[id];
        }

        /// Gets the Contact Centers of a specific type (Household, Workplace, ...).
        const std::vector<std::shared_ptr<ContactCenter>>& CRefContactCentersOfType (stride::ContactType::Id id) const
        {
                return m_CC_OfType[id];
        }

        /// Gets ID of this Location.
        unsigned int GetID() const { return m_id; }

        /// Gets a vector with the outgoing cities which people are commuting to + the proportion.
        const std::vector<std::pair<Location*, double>>& GetIncomingCommuningCities() const;

        /// Calculates number of incomming commuters, given the fraction of the population that commutes.
        int GetIncomingCommuterCount(double fractionCommuters) const;

        /// Gets the number of people infected in the contactpools at this location.
        double GetInfectedCount() const;

        /// Gets the name.
        std::string GetName() const { return m_name; }

        /// Returns outgoing cities which people are commuting to + the proportion.
        const std::vector<std::pair<Location*, double>>& GetOutgoingCommutingCities() const;

        /// Calculates number of outgoing commuters, given the fraction of the population that commutes.
        int GetOutgoingCommuterCount(double fractionCommuters) const;

        /// Gets the absolute population.
        unsigned int GetPopCount() const { return m_pop_count; }

        /// Gets the province.
        unsigned int GetProvince() const { return m_province; }

        /// Gets the relative population.
        double GetRelativePopulationSize() const;

        /// Sets the Coordinate of this Location.
        void SetCoordinate(const Coordinate& coordinate) { m_coordinate = coordinate; }

        /// Calculates this location's population count using its relative population and the total population count.
        void SetPopCount(unsigned int totalPopCount);

        /// Sets the relative population, which will be later used by @see CalculatePopulation.
        void SetRelativePopulation(double relativePopulation);

private:
        unsigned int m_id = 0U;       ///< Id.
        std::string  m_name;          ///< Name.
        unsigned int m_province;      ///< Province id.
        unsigned int m_pop_count;     ///< Population count (number of individuals) at this Location.
        double       m_pop_fraction;  ///< Fraction of whole population at thois Location.
        Coordinate   m_coordinate;    ///< Coordinate of the Location.

        /// Incomming commutes stored as pair of Location and fraction of population at that Location.
        std::vector<std::pair<Location*, double>> m_inCommuteLocations;

        ///< Outgoing commutes stored as pair of Location and fraction of population to this this Location.
        std::vector<std::pair<Location*, double>> m_outCommuteLocations;

        ///< Stores the contact centers indexed by their type.
        stride::ContactType::IdSubscriptArray<std::vector<std::shared_ptr<ContactCenter>>> m_CC_OfType;
};

} // namespace geopop
