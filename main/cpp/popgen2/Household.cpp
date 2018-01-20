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
 * Implementation file for the Household class.
 */

#include "Household.h"

namespace stride {
namespace generator {

std::size_t Household::g_id_generator = 1;

Household::Household()
{
        m_id = g_id_generator;
        g_id_generator++;
        m_size = 0;
}

void Household::AddMember(std::size_t member_id)
{
        m_members.push_back(member_id);
        m_size++;
}

void Household::SetCityID(std::size_t city_id) { m_city_id = city_id; }

} // namespace generator
} // namespace stride
