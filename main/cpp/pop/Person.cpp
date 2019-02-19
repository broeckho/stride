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

#include "contact/ContactType.h"
#include "pop/Age.h"

namespace stride {

using namespace std;
using namespace stride::ContactType;

void Person::Update(bool isWorkOff, bool isSchoolOff, bool adaptiveSymptomaticBehavior)
{
        // Update health and disease status
        m_health.Update();

        // Update presence in contact pools.
        if (m_health.IsSymptomatic() && adaptiveSymptomaticBehavior) {
                m_in_pools[Id::K12School]          = false;
                m_in_pools[Id::College]            = false;
                m_in_pools[Id::Workplace]          = false;
                m_in_pools[Id::PrimaryCommunity]   = false;
                m_in_pools[Id::SecondaryCommunity] = false;
        } else if (isWorkOff || (m_age <= MinAdultAge() && isSchoolOff)) {
                m_in_pools[Id::K12School]          = false;
                m_in_pools[Id::College]            = false;
                m_in_pools[Id::Workplace]          = false;
                m_in_pools[Id::PrimaryCommunity]   = true;
                m_in_pools[Id::SecondaryCommunity] = false;
        } else {
                m_in_pools[Id::K12School]          = true;
                m_in_pools[Id::College]            = true;
                m_in_pools[Id::Workplace]          = true;
                m_in_pools[Id::PrimaryCommunity]   = false;
                m_in_pools[Id::SecondaryCommunity] = true;
        }
}

} // namespace stride
