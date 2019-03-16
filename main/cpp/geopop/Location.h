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
#include "geopop/ContactCenter.h"
#include "geopop/Coordinate.h"

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
 * Location for use within the GeoGrid, contains Coordinate and ContactCenters.
 */
class Location
{
public:
        /// Parametrized constructor with population count.
        Location(unsigned int id, unsigned int province, Coordinate coordinate = Coordinate(0.0, 0.0),
                 std::string name = "", unsigned int popCount = 0U);

        /// Perform a full comparison with the other location.
        bool operator==(const Location& other) const;

        /// Add a ContactCenter.
        void AddCenter(const std::shared_ptr<ContactCenter>& contactCenter);

        /// Adds a Location and a proportion to the incoming commute vector.
        /// I.e. fraction of commuting population at otherLocation commuting to this Location.
        void AddIncomingCommute(std::shared_ptr<Location> otherLocation, double fraction);

        /// Adds a Location and a fraction to the outgoing commute vector.
        /// I.e. fraction of commuting population at this Location commuting to otherLocation.
        void AddOutgoingCommute(std::shared_ptr<Location> otherLocation, double fraction);

        /// Gets the Coordinate of this Location.
        const Coordinate GetCoordinate() const { return m_coordinate; }

        /// Gets ID of this Location.
        unsigned int GetID() const { return m_id; }

        /// Calculates number of incomming commuters, given the fraction of the population that commutes.
        int GetIncomingCommuteCount(double fractionCommuters) const;

        /// Gets the number of people infected in the contactpools at this location.
        unsigned int GetInfectedCount() const;

        /// Gets the name.
        std::string GetName() const { return m_name; }

        /// Calculates number of outgoing commuters, given the fraction of the population that commutes.
        unsigned int GetOutgoingCommuteCount(double fractionCommuters) const;

        /// Gets the absolute population.
        unsigned int GetPopCount() const { return m_pop_count; }

        /// Gets the province.
        unsigned int GetProvince() const { return m_province; }

        /// Gets the relative population.
        double GetRelativePop() const;

        /// Sets the Coordinate of this Location.
        void SetCoordinate(const Coordinate& coordinate) { m_coordinate = coordinate; }

        /// Calculates this location's population count using its relative population and the total population count.
        void SetPopCount(unsigned int totalPopCount);

        /// Sets the relative population, which will be later used by @see CalculatePopulation.
        void SetRelativePop(double relativePopulation);

public:
        /// References incoming commute Locations + fraction of commutes to that Location.
        const std::vector<std::pair<Location*, double>>& CRefIncomingCommutes() const { return m_inCommutes; }

        /// References outgoing commute Locations + fraction of commutes to that Location.
        const std::vector<std::pair<Location*, double>>& CRefOutgoingCommutes() const { return m_outCommutes; }

        /// Reference the Contact Centers of a specific type id (Household, Workplace, ...).
        const std::vector<std::shared_ptr<ContactCenter>>& CRefCenters(stride::ContactType::Id id) const
        {
                return m_cc[id];
        }

        /// Reference the Contact Centers of a specific type id (Household, Workplace, ...).
        std::vector<std::shared_ptr<ContactCenter>>& RefCenters(stride::ContactType::Id id) { return m_cc[id]; }

private:
        Coordinate   m_coordinate;   ///< Coordinate of the Location.
        unsigned int m_id = 0U;      ///< Id.
        std::string  m_name;         ///< Name.
        unsigned int m_pop_count;    ///< Population count (number of individuals) at this Location.
        double       m_pop_fraction; ///< Fraction of whole population at this Location.
        unsigned int m_province;     ///< Province id.

        /// Incomming commutes stored as pair of Location and fraction of population at that Location.
        std::vector<std::pair<Location*, double>> m_inCommutes;

        ///< Outgoing commutes stored as pair of Location and fraction of population to this this Location.
        std::vector<std::pair<Location*, double>> m_outCommutes;

        ///< Stores the contact centers indexed by their type.
        stride::ContactType::IdSubscriptArray<std::vector<std::shared_ptr<ContactCenter>>> m_cc;
};

} // namespace geopop
