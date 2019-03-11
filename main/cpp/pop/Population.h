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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the core Population class
 */

#pragma once

#include "contact/ContactPoolSys.h"
#include "contact/ContactType.h"
#include "pop/Person.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace geopop {
class GeoGrid;
}

namespace stride {

namespace util {
class RnMan;
}

/**
 * Key Data structure: container for
 * (a) all individuals in the population
 * (b) the ContactPoolSys wchich is used to loop over ContactPools of each type
 * (c) (if present) geographical grid of Locations with ContactCenters at that location.
 */
class Population : public util::SegmentedVector<Person>
{
public:
        /// Create a population initialized by the configuration in property tree.
        static std::shared_ptr<Population> Create(const boost::property_tree::ptree& configPt, util::RnMan& rnManager,
                                                  std::shared_ptr<spdlog::logger> stride_logger = nullptr);

        /// For use in python environment: create using configuration string i.o ptree.
        static std::shared_ptr<Population> Create(const std::string& configString, util::RnMan& rnManager,
                                                  std::shared_ptr<spdlog::logger> stride_logger = nullptr);

        /// Create an empty Population, used in gengeopop.
        static std::shared_ptr<Population> Create();

public:
        /// Add a new contact pool of a given type
        ContactPool* CreateContactPool(ContactType::Id typeId);

        /// Create Person in the population.
        Person* CreatePerson(unsigned int id, double age, unsigned int householdId, unsigned int k12SchoolId,
                             unsigned int college, unsigned int workId, unsigned int primaryCommunityId,
                             unsigned int secondaryCommunityId);
        /// Get the cumulative number of cases.
        unsigned int GetInfectedCount() const;

        /// Return the contactlogger.
        std::shared_ptr<spdlog::logger>& GetContactLogger() { return m_contact_logger; }

        /// The ContactPoolSys of the simulator.
        ContactPoolSys& GetContactPoolSys() { return m_pool_sys; }

        /// The ContactPoolSys of the simulator.
        const ContactPoolSys& GetContactPoolSys() const { return m_pool_sys; }

        /// Get the GeoGrid associated with this population (may be a nullptr).
        std::shared_ptr<geopop::GeoGrid> GetGeoGrid() const { return m_geoGrid; }

private:
        /// Non-trivial default constructor.
        Population();

        friend class DefaultPopBuilder;
        friend class GeoPopBuilder;
        friend class ImportPopBuilder;

private:
        ContactPoolSys                   m_pool_sys;       ///< Holds vector of ContactPools of different types.
        std::shared_ptr<spdlog::logger>  m_contact_logger; ///< Logger for contact/transmission.
        std::shared_ptr<geopop::GeoGrid> m_geoGrid;        ///< Associated geoGrid may be nullptr.

private:
        /// The contact pool counters (one per type of pool) for assigning pool IDs. Counters
        /// generate a non zero UID that's unique per type of pool, so <type, UID> uniquely
        /// detemines the pool. UID zero means 'NA" e.g. wor[lace UID for a K12school student
        /// wiil be zero. As a defensive measure, the ContactPoolSys gets initialized with
        /// (for each type) an empty pool in the vecotor storing the contact pools. As a
        /// consequence, one has:
        /// if UID != 0 then ContactPoolsSys[type][UID].GetId() == UID for all type
        /// the index in the vector with pools is identical to the pool's UID.
        ContactType::IdSubscriptArray<unsigned int> m_currentContactPoolId;
};

} // namespace stride
