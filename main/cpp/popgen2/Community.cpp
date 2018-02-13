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

#include "Community.h"

#include <algorithm>
#include <iostream>

namespace stride {
namespace generator {

using namespace std;

Community::Community(CommunityType community_type, size_t city_id, unsigned int size)
    : m_community_type(community_type), m_city_id(city_id), m_size(size)
{
        m_community_id = UIDgenerator();
        UIDgenerator()++;
}

Community::Community(size_t id, CommunityType community_type, size_t city_id, unsigned int size)
    : m_community_id(id), m_community_type(community_type), m_city_id(city_id), m_size(size)
{
        UIDgenerator() = max(m_community_id, UIDgenerator()) + 1;
}
size_t& Community::UIDgenerator()
{
        static size_t id = 1; // First id is 1! Needs to be as 0 implies "absent from community".
        return id;
}

} // namespace generator
} // namespace stride