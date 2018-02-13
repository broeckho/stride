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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 */

/**
 * @file
 * Header file for the Person class used by the population generator.
 */

#include <boost/mpl/size_t.hpp>

#include <cstddef>

#include "pop/Person.h"

namespace stride {
namespace generator {

class PopulationGenerator;

class GeneratorPerson
{
public:
        ///
        GeneratorPerson();

        ///
        explicit GeneratorPerson(std::size_t id);

        ///
        GeneratorPerson(std::size_t id, unsigned int age, std::size_t household_id, std::size_t school_id,
                        std::size_t university_id, std::size_t work_id, std::size_t primary_community,
                        std::size_t secondary_community);

        ///
        explicit operator Person() const
        {
                size_t school_id = m_school_id;
                if (school_id == 0) {
                        school_id = m_university_id;
                }
                return {static_cast<unsigned int>(m_id),
                        static_cast<double>(m_age),
                        static_cast<unsigned int>(m_household_id),
                        static_cast<unsigned int>(school_id),
                        static_cast<unsigned int>(m_work_id),
                        static_cast<unsigned int>(m_primary_community),
                        static_cast<unsigned int>(m_secondary_community),
                        0,
                        0,
                        0,
                        0};
        }

        ///
        bool operator==(const GeneratorPerson& other) const;

        ///
        void SetAge(unsigned int age);

        ///
        void SetHouseholdID(std::size_t id);

        ///
        void SetSchoolID(std::size_t id);

        ///
        void SetUniversityID(std::size_t id);

        ///
        void SetWorkID(std::size_t id);

        ///
        void SetPrimaryCommunityID(std::size_t id);

        ///
        void SetSecondaryCommunityID(std::size_t id);

        ///
        void SetCityID(std::size_t id);

        ///
        std::size_t GetID() const { return m_id; }

        ///
        unsigned int GetAge() const { return m_age; }

        ///
        std::size_t GetHouseholdID() const { return m_household_id; }

        ///
        std::size_t GetSchoolID() const { return m_school_id; }

        ///
        std::size_t GetUniversityID() const { return m_university_id; }

        ///
        std::size_t GetWorkID() const { return m_work_id; }

        ///
        std::size_t GetPrimaryCommunityID() const { return m_primary_community; }

        ///
        std::size_t GetSecondaryCommunityID() const { return m_secondary_community; }

        ///
        std::size_t GetCityID() const { return m_city_id; }

private:
        std::size_t  m_id;  ///< A unique ID for the person
        unsigned int m_age; ///< The person's age

        std::size_t m_household_id;        ///< The household ID to which the person belongs
        std::size_t m_school_id;           ///< The school ID which the person attends. Default 0.
        std::size_t m_university_id;       ///< The university ID which the person attends. Default 0.
        std::size_t m_work_id;             ///< The work ID which the person attends. Default 0.
        std::size_t m_primary_community;   ///< The primary community ID which the person attends. Default 0.
        std::size_t m_secondary_community; ///< The secondary community ID which the person attends. Default 0.

        std::size_t m_city_id; ///< The city ID in which the person lives.
};

} // namespace generator
} // namespace stride