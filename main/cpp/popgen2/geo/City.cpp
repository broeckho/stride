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

#include "City.h"

namespace stride {
namespace generator {

std::size_t City::g_id_generator = 1;

City::City(std::size_t id, double population, std::string name, std::size_t province, Point2D location, Point2D dummy)
    : m_id(id), m_population(population), m_name(std::move(name)), m_province(province), m_location(location),
      m_dummy_location(dummy)
{
        g_id_generator = std::max(m_id, g_id_generator) + 1;
}

City::City(double population, std::string name, std::size_t province, Point2D location, Point2D dummy)
    : m_population(population), m_name(std::move(name)), m_province(province), m_location(location),
      m_dummy_location(dummy)
{
        m_id = g_id_generator;
        g_id_generator++;
}

bool City::operator==(const City& other) const
{
        return m_id == other.m_id && m_population == other.m_population && m_dummy_location == other.m_dummy_location &&
               m_location == other.m_location && m_name == other.m_name && m_province == other.m_province;
}

} // namespace generator
} // namespace stride