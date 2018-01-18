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
 * Header for the core Cluster class.
 */

#include "core/ClusterType.h"
#include "core/ContactProfile.h"
#include "core/ContactProfiles.h"
#include "core/LogMode.h"
#include "pop/Person.h"

#include <array>
#include <vector>

namespace stride {

/**
 * Represents a location for social contacts, an group of people.
 */
class Cluster
{
public:
        /// Constructor
        Cluster(std::size_t cluster_id, ClusterType::Id cluster_type, const ContactProfiles& profiles);

        /// No copying: too big.
        Cluster(const Cluster&) = delete;

        /// Moving is ok.
        Cluster(Cluster&&) = default;

        // No assignment: too big.
        Cluster& operator=(const Cluster&) = delete;

        // Move assignment is ok.
        Cluster& operator=(Cluster&&) = default;

        /// Add the given Person to the Cluster.
        void AddMember(Person* p);

        /// Return the type of this cluster.
        ClusterType::Id GetClusterType() const { return m_cluster_type; }
        //
        Person* GetMember(unsigned int index) const { return m_members[index].first; }

        /// Return number of persons in this cluster.
        std::size_t GetSize() const { return m_members.size(); }

        /// Get basic contact rate in this cluster.
        double GetContactRate(const Person* p) const;

private:
        /// Sort members w.r.t. health status.
        /// Sort order: exposed/infected/recovered, susceptible, immune).
        std::tuple<bool, size_t> SortMembers();

        /// Infector calculates contacts and transmissions.
        template <LogMode::Id LL, bool TIC, typename LIP, bool TO>
        friend class Infector;

        /// Calculate which members are present in the cluster on the current day.
        void UpdateMemberPresence();

private:
        std::size_t m_cluster_id;                        ///< The ID of the Cluster (for logging purposes).
        ClusterType::Id m_cluster_type;                  ///< The type of the Cluster (for logging purposes).
        std::size_t m_index_immune;                      ///< Index of the first immune member in the Cluster.
        std::vector<std::pair<Person*, bool>> m_members; ///< Container with pointers to Cluster members.
        const ContactProfile& m_profile;                 ///< Contact pattern.
};

} // namespace stride
