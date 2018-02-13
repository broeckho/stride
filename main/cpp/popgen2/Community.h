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

#include <cstddef>

namespace stride {
namespace generator {

enum class CommunityType
{
        School,
        University,
        Work,
        Primary,
        Secondary,
        Null
};

class Community
{
public:
        ///
        Community() = default;

        ///
        Community(CommunityType community_type, std::size_t city_id, unsigned int size);

        ///
        Community(std::size_t id, CommunityType community_type, std::size_t city_id, unsigned int size);

        ///
        std::size_t GetCityID() { return m_city_id; }

        ///
        CommunityType GetCommunityType() { return m_community_type; }

        ///
        std::size_t GetID() { return m_community_id; }

        ///
        unsigned int GetSize() { return m_size; }

private:
        static std::size_t& UIDgenerator();

private:
        std::size_t   m_community_id;   ///< A unique ID for the community
        CommunityType m_community_type; ///< The type of community
        std::size_t   m_city_id;        ///< The ID of the city in which the community is located
        unsigned int  m_size;           ///< The maximum number of people that can part of this community.
};

} // namespace generator
} // namespace stride
