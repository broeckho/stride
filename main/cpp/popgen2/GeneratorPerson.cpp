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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 */

#include "GeneratorPerson.h"

#include <iostream>

namespace stride {
namespace generator {

GeneratorPerson::GeneratorPerson()
{
        m_id                  = 0;
        m_household_id        = 0;
        m_school_id           = 0;
        m_university_id       = 0;
        m_work_id             = 0;
        m_primary_community   = 0;
        m_secondary_community = 0;
}

GeneratorPerson::GeneratorPerson(std::size_t id)
{
        m_id                  = id;
        m_household_id        = 0;
        m_school_id           = 0;
        m_university_id       = 0;
        m_work_id             = 0;
        m_primary_community   = 0;
        m_secondary_community = 0;
}

GeneratorPerson::GeneratorPerson(std::size_t id, unsigned int age, std::size_t household_id, std::size_t school_id,
                                 std::size_t university_id, std::size_t work_id, std::size_t primary_community,
                                 std::size_t secondary_community)
    : m_id(id), m_age(age), m_household_id(household_id), m_school_id(school_id), m_university_id(university_id),
      m_work_id(work_id), m_primary_community(primary_community), m_secondary_community(secondary_community)
{
}

bool GeneratorPerson::operator==(const GeneratorPerson& other) const
{
        return m_id == other.m_id && m_age == other.m_age && m_household_id == other.m_household_id &&
               m_school_id == other.m_school_id && m_university_id == other.m_university_id &&
               m_work_id == other.m_work_id && m_primary_community == other.m_primary_community &&
               m_secondary_community == other.m_secondary_community;
}

void GeneratorPerson::SetAge(unsigned int age) { m_age = age; }

void GeneratorPerson::SetHouseholdID(std::size_t id) { m_household_id = id; }

void GeneratorPerson::SetSchoolID(std::size_t id) { m_school_id = id; }

void GeneratorPerson::SetUniversityID(std::size_t id) { m_university_id = id; }

void GeneratorPerson::SetWorkID(std::size_t id) { m_work_id = id; }

void GeneratorPerson::SetPrimaryCommunityID(std::size_t id) { m_primary_community = id; }

void GeneratorPerson::SetSecondaryCommunityID(std::size_t id) { m_secondary_community = id; }

void GeneratorPerson::SetCityID(std::size_t id) { m_city_id = id; }

} // namespace generator
} // namespace stride
