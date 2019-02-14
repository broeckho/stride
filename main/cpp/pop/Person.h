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
 * Header file for the Person class.
 */

#pragma once

#include "disease/Health.h"
#include "pool/ContactPoolType.h"
#include "pool/IdSubscriptArray.h"

#include <cstddef>

namespace stride {

/**
 * Store and handle person data.
 */
class Person
{
public:
        /// Default construction (for population vector).
        Person()
            : m_age(0.0), m_gender('M'), m_health(), m_id(0), m_is_participant(), m_pool_ids(),
              m_in_pools()
        {
        }

        /// Constructor: set the person data.
        Person(unsigned int id, double age, unsigned int householdId, unsigned int k12SchoolId, unsigned int collegeId,
               unsigned int workId, unsigned int primaryCommunityId, unsigned int secondaryCommunityId)
            : m_age(age), m_gender('M'), m_health(), m_id(id),
              m_is_participant(false), m_pool_ids{householdId, k12SchoolId,        collegeId,
                                                  workId,      primaryCommunityId, secondaryCommunityId},
              m_in_pools(true)
        {
        }

        /// Is this person not equal to the given person?
        bool operator!=(const Person& p) const { return p.m_id != m_id; }

        /// Get the age.
        double GetAge() const { return m_age; }

        /// Get ID of contactpool_type
        unsigned int GetPoolId(const ContactPoolType::Id& poolType) const { return m_pool_ids[poolType]; }

        /// Return person's health status.
        Health& GetHealth() { return m_health; }

        /// Return person's health status.
        const Health& GetHealth() const { return m_health; }

        /// Get the id.
        unsigned int GetId() const { return m_id; }

        /// Set the id.
        void SetId(unsigned int id) { m_id = id; }

        /// Check if a person is present today in a given contact pool
        bool IsInPool(const ContactPoolType::Id& poolType) const { return m_in_pools[poolType]; }

        /// Does this person participates in the social contact study?
        bool IsSurveyParticipant() const { return m_is_participant; }

        /// Participate in social contact study and log person details
        void ParticipateInSurvey() { m_is_participant = true; }

        /// Update the health status and presence in contact pools.
        void Update(bool isWorkOff, bool isSchoolOff, bool adaptiveSymptomaticBehavior);

        ///
        void Update(Person* p);

        /// Set the age of the person
        void SetAge(unsigned int newAge) { m_age = newAge; }

        unsigned int GetHouseholdId() { return GetPoolId(ContactPoolType::Id::Household); }

        void SetHouseholdId(unsigned int household_id) { SetPoolId(ContactPoolType::Id::Household, household_id); }

        /// Returns the id of the K12School CP
        unsigned int GetK12SchoolId() const { return GetPoolId(ContactPoolType::Id::K12School); }

        /// Sets the id of the K12School CP
        void SetK12SchoolId(unsigned int k12school_id) { SetPoolId(ContactPoolType::Id::K12School, k12school_id); }

        /// Returns the id of the College CP
        unsigned int GetCollegeId() const { return GetPoolId(ContactPoolType::Id::College); }

        /// Sets the id of the College CP
        void SetCollegeId(unsigned int college_id) { SetPoolId(ContactPoolType::Id::College, college_id); }

        /// Gets the id of the Work CP
        unsigned int GetWorkId() const { return GetPoolId(ContactPoolType::Id::Work); }

        /// Sets the id of the Work CP
        void SetWorkId(unsigned int work_id) { SetPoolId(ContactPoolType::Id::Work, work_id); }

        /// Returns the id of the PrimaryCommunity CP
        unsigned int GetPrimaryCommunityId() const { return GetPoolId(ContactPoolType::Id::PrimaryCommunity); }

        /// Sets the id of the PrimaryCommunity CP
        void SetPrimaryCommunityId(unsigned int primary_community_id)
        {
                SetPoolId(ContactPoolType::Id::PrimaryCommunity, primary_community_id);
        }

        /// Returns the id of the SecondaryCommunity CP
        unsigned int GetSecondaryCommunityId() const { return GetPoolId(ContactPoolType::Id::SecondaryCommunity); }

        /// Sets the id of the SecondaryCommunity CP
        void SetSecondaryCommunityId(unsigned int secondary_community_id)
        {
                SetPoolId(ContactPoolType::Id::SecondaryCommunity, secondary_community_id);
        }

        /// Sets (for the type of ContactPool) the Id of the ContactPool the person belongs yo..
        void SetPoolId(ContactPoolType::Id type, unsigned int poolId)
        {
                m_pool_ids[type] = poolId;
                m_in_pools[type] = (poolId != 0); // Means present in Household, absent elsewhere.
        }

private:
        double       m_age;            ///< The age.
        char         m_gender;         ///< Gender.
        Health       m_health;         ///< Health info for this person.
        unsigned int m_id;             ///< The id.
        bool         m_is_participant; ///< Is participating in the social contact study

        ///< Ids (school, work, etc) of pools you belong to Id value 0 means you do not belong to any
        ///< pool of that type (e.g. school and work are mutually exclusive).
        ContactPoolType::IdSubscriptArray<unsigned int> m_pool_ids;

        ///< Is person present/absent in pools of each of the types (school, work, etc)?
        ContactPoolType::IdSubscriptArray<bool> m_in_pools;
};

} // namespace stride
