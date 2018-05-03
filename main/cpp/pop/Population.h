#pragma once
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

#include "pool/ContactPoolSys.h"
#include "pop/Person.h"
#include "util/Any.h"
#include "util/SegmentedVector.h"

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace stride {

/**
 * Container for persons in population.
 */
class Population : public util::SegmentedVector<Person>
{
public:
        /// Create a population initialized by the configuration in property tree.
        static std::shared_ptr<Population> Create(const boost::property_tree::ptree& configPt);

        /// For use in python environment: create using configuration string i.o ptree.
        static std::shared_ptr<Population> Create(const std::string& configString);

        ///
        unsigned int GetAdoptedCount() const;

        /// Get the cumulative number of cases.
        unsigned int GetInfectedCount() const;

        ///
        std::shared_ptr<spdlog::logger>& GetContactLogger() { return m_contact_logger; }

        /// The ContactPoolSys of the simulator.
        ContactPoolSys& GetContactPoolSys() { return m_pool_sys; }

        /// The ContactPoolSys of the simulator.
        const ContactPoolSys& GetContactPoolSys() const { return m_pool_sys; }

private:
        ///
        Population() = default;

        /// New Person in the population.
        void CreatePerson(unsigned int id, double age, unsigned int householdId, unsigned int schoolId,
                          unsigned int workId, unsigned int primaryCommunityId, unsigned int secondaryCommunityId,
                          Health health, const boost::property_tree::ptree& beliefPt, double riskAverseness = 0);

        ///
        template <typename BeliefPolicy>
        void NewPerson(unsigned int id, double age, unsigned int householdId, unsigned int schoolId,
                       unsigned int workId, unsigned int primaryCommunityId, unsigned int secondaryCommunityId,
                       Health health, const boost::property_tree::ptree& beliefPt, double riskAverseness = 0);

        friend class PopBuilder;

private:
        util::Any                       beliefs_container; ///< Holds belief data for the persons.
        ContactPoolSys                  m_pool_sys;        ///< Holds vector of ContactPools of different types.
        std::shared_ptr<spdlog::logger> m_contact_logger;  ///< Logger for contact/transmission.
};

} // namespace stride
