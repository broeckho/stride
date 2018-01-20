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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the Household class which is used by the population generator.
 */

#include <cstddef>
#include <vector>

namespace stride {
namespace generator {

class Household
{
public:
        ///
        Household();

        ///
        static std::size_t g_id_generator;

        /// Add a member to the household.
        void AddMember(std::size_t member_id);

        ///
        void SetCityID(std::size_t city_id);

        ///
        std::vector<std::size_t> GetMembers() { return m_members; }

        ///
        std::size_t GetID() { return m_id; }

        ///
        std::size_t GetSize() { return m_size; }

        ///
        std::size_t GetCityID() { return m_city_id; }

private:
        std::size_t m_id;                   ///< A unique ID of the household
        std::size_t m_size;                 ///< The size of the household a.k.a. the number of members.
        std::vector<std::size_t> m_members; ///< A vector of ID's referring to the ID of a person.
        std::size_t m_city_id;              ///< The ID of the city in which the household is located.
};

} // namespace generator
} // namespace stride
