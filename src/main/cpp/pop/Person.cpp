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

#include "core/ClusterType.h"
#include "pop/Age.h"

#include <stdexcept>

namespace stride {

using namespace std;

unsigned int Person::GetClusterId(ClusterType cluster_type) const
{
	switch (cluster_type) {
	case ClusterType::Household:
		return m_household_id;
	case ClusterType::School:
		return m_school_id;
	case ClusterType::Work:
		return m_work_id;
	case ClusterType::PrimaryCommunity:
		return m_primary_community_id;
	case ClusterType::SecondaryCommunity:
		return m_secondary_community_id;
	default:
		throw runtime_error(string(__func__) + "> Should not reach default.");
	}
}

bool Person::IsInCluster(ClusterType c) const
{
	switch (c) {
	case ClusterType::Household:
		return m_at_household;
	case ClusterType::School:
		return m_at_school;
	case ClusterType::Work:
		return m_at_work;
	case ClusterType::PrimaryCommunity:
		return m_at_primary_community;
	case ClusterType::SecondaryCommunity:
		return m_at_secondary_community;
	default:
		throw runtime_error(string(__func__) + "> Should not reach default.");
	}
}

void Person::Update(bool is_work_off, bool is_school_off)
{
	m_health.Update();

	// Update presence in clusters.
	if (m_health.IsSymptomatic()) {
		m_at_school = false;
		m_at_work = false;
		m_at_secondary_community = false;
		m_at_primary_community = false;
		// TODO set at_home_due_to_illness?
	} else if (is_work_off || (m_age <= MinAdultAge() && is_school_off)) {
		m_at_school = false;
		m_at_work = false;
		m_at_secondary_community = false;
		m_at_primary_community = true;
	} else {
		m_at_school = true;
		m_at_work = true;
		m_at_secondary_community = true;
		m_at_primary_community = false;
	}
}

void Person::Update(Person* p)
{
	// TODO update beliefs
}

} // end_of_namespace
