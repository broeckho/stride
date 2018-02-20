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

#include "Person.h"
#include "pop/Age.h"

namespace stride {

using namespace std;

unsigned int Person::GetContactPoolId(const ContactPoolType::Id &cluster_type) const
{
        unsigned int ret;
        switch (cluster_type) {
        case ContactPoolType::Id::Household: ret = m_household_id; break;
        case ContactPoolType::Id::School: ret = m_school_id; break;
        case ContactPoolType::Id::Work: ret = m_work_id; break;
        case ContactPoolType::Id::PrimaryCommunity: ret = m_primary_community_id; break;
        case ContactPoolType::Id::SecondaryCommunity: ret = m_secondary_community_id; break;
        }
        return ret;
}

bool Person::IsInContactPool(const ContactPoolType::Id &c) const
{
        bool ret;
        switch (c) {
        case ContactPoolType::Id::Household: ret = m_at_household; break;
        case ContactPoolType::Id::School: ret = m_at_school; break;
        case ContactPoolType::Id::Work: ret = m_at_work; break;
        case ContactPoolType::Id::PrimaryCommunity: ret = m_at_primary_community; break;
        case ContactPoolType::Id::SecondaryCommunity: ret = m_at_secondary_community; break;
        }
        return ret;
}

void Person::Update(bool is_work_off, bool is_school_off)
{
        m_health.Update();

        // Update presence in clusters.
        if (m_health.IsSymptomatic()) {
                m_at_school              = false;
                m_at_work                = false;
                m_at_secondary_community = false;
                m_at_primary_community   = false;
                // TODO set at_home_due_to_illness?
        } else if (is_work_off || (m_age <= MinAdultAge() && is_school_off)) {
                m_at_school              = false;
                m_at_work                = false;
                m_at_secondary_community = false;
                m_at_primary_community   = true;
        } else {
                m_at_school              = true;
                m_at_work                = true;
                m_at_secondary_community = true;
                m_at_primary_community   = false;
        }
}

void Person::Update(Person*)
{
        // TODO update beliefs
}

} // namespace stride
